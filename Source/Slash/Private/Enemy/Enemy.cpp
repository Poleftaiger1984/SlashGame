// Fill out your copyright notice in the Description page of Project Settings.

#pragma	once

#include "Enemy/Enemy.h"
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CharacterTypes.h"

#include "Slash/DebugMacros.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::Die(const FVector& ImpactPoint)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	FName DeathSectionName = DirectionalDeathSelection(ImpactPoint, DeathSectionName);

	PlayDeathMontage(DeathSectionName);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(7.f);
}

FName AEnemy::DirectionalDeathSelection(const FVector& ImpactPoint, FName& DeathSectionName)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double AngleOfAttack = FMath::Acos(CosTheta);
	//Convert from radians to degrees
	AngleOfAttack = FMath::RadiansToDegrees(AngleOfAttack);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	//Random Selection of animation sections between the different animations for all 4 different angles of attack
	if (CrossProduct.Z < 0)
	{
		AngleOfAttack *= -1.f;
	}
	int32 const SelectionBack = FMath::RandRange(0, 2);
	switch (SelectionBack)
	{
	case 0:
		DeathSectionName = FName("DeathBack1");
		DeathPose = EDeathPose::EDP_DeathBack1;
		break;
	case 1:
		DeathSectionName = FName("DeathBack2");
		DeathPose = EDeathPose::EDP_DeathBack2;
		break;
	case 2:
		DeathSectionName = FName("DeathBack3");
		DeathPose = EDeathPose::EDP_DeathBack3;
		break;
	default:
		break;
	}

	if (AngleOfAttack >= -45.f && AngleOfAttack < 45.f)
	{
		int32 const SelectionForward = FMath::RandRange(0, 2);
		switch (SelectionForward)
		{
		case 0:
			DeathSectionName = FName("DeathForward1");
			DeathPose = EDeathPose::EDP_DeathForward1;
			break;
		case 1:
			DeathSectionName = FName("DeathForward2");
			DeathPose = EDeathPose::EDP_DeathForward2;
			break;
		case 2:
			DeathSectionName = FName("DeathForward3");
			DeathPose = EDeathPose::EDP_DeathForward3;
		default:
			break;
		}
	}
	else if (AngleOfAttack >= -135.f && AngleOfAttack < -45.f)
	{
		int32 const SelectionLeft = FMath::RandRange(0, 1);
		switch (SelectionLeft)
		{
		case 0:
			DeathSectionName = FName("DeathLeft1");
			DeathPose = EDeathPose::EDP_DeathLeft1;
			break;
		case 1:
			DeathSectionName = FName("DeathLeft2");
			DeathPose = EDeathPose::EDP_DeathLeft2;
			break;
		default:
			break;
		}
	}
	else if (AngleOfAttack >= 45.f && AngleOfAttack < 135.f)
	{
		int32 const SelectionRight = FMath::RandRange(0, 1);
		switch (SelectionRight)
		{
		case 0:
			DeathSectionName = FName("DeathRight1");
			DeathPose = EDeathPose::EDP_DeathRight1;
			break;
		case 1:
			DeathSectionName = FName("DeathRight2");
			DeathPose = EDeathPose::EDP_DeathRight2;
			break;
		default:
			break;
		}
	}
	return DeathSectionName;
}

bool AEnemy::InTargetRange(TObjectPtr<AActor> Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(TObjectPtr<AActor> Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);
	EnemyController->MoveTo(MoveRequest);
}

TObjectPtr<AActor> AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	if (PatrolTargets.Num() > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing) return;
	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::PlayDeathMontage(const FName& DeathSectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(DeathSectionName, DeathMontage);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		//Outside Combat radius, lose interest
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		//Outside attack range, chase player
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		//Inside attack range, attack player
		EnemyState = EEnemyState::EES_Attacking;
		//ToDo: AttackMontage
	}

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die(ImpactPoint); //To Implement Directional Death Animations
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lower Impact point to the enemy actors z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); //GetSafeNormal normalizes the vector

	// Forward * ToHit = |Forward| |ToHit| * cos(è)
	// |Forward| = 1, |ToHit| = 1 so Forward * ToHit = cos(è)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	//Take the inverse cosine of cos gets us the angle è
	double AngleOfAttack = FMath::Acos(CosTheta);
	//Convert from radians to degrees
	AngleOfAttack = FMath::RadiansToDegrees(AngleOfAttack);

	//If CrossProduct points down then è should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		AngleOfAttack *= -1.f;
	}
	FName Section("FromBack");

	if (AngleOfAttack >= -45.f && AngleOfAttack < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (AngleOfAttack >= -135.f && AngleOfAttack < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (AngleOfAttack >= 45.f && AngleOfAttack < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
}

//TakeDamage is called whenever we ApplyDamage
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->RecieveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
	return DamageAmount;
}