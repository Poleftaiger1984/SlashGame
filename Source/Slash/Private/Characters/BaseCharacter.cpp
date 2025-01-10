// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else if(Hitter) Die(Hitter->GetActorLocation());

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die(const FVector& ImpactPoint)
{
	Tags.Add(FName("Dead"));
	DirectionalDeathSelection(ImpactPoint);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayDeathMontage(const FName& DeathSectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(DeathSectionName, DeathMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

void ABaseCharacter::DirectionalDeathSelection(const FVector& ImpactPoint)
{
	FName DeathSectionName;
	int32 Selection;
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
	Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSectionsBack);
	ChooseDeathPose(AngleOfAttack, Selection);

	if (AngleOfAttack >= -45.f && AngleOfAttack < 45.f)
	{
		Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSectionsForward);
		ChooseDeathPose(AngleOfAttack, Selection);
	}
	else if (AngleOfAttack >= -135.f && AngleOfAttack < -45.f)
	{
		Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSectionsRight);
		ChooseDeathPose(AngleOfAttack, Selection);
	}
	else if (AngleOfAttack >= 45.f && AngleOfAttack < 135.f)
	{
		Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSectionsLeft);
		ChooseDeathPose(AngleOfAttack, Selection);
	}

}

void ABaseCharacter::ChooseDeathPose(double Angle, int32 Selection)
{
	TEnumAsByte<EDeathPose> Pose(Selection);
	DeathPose = Pose;

	if (Angle >= -45.f && Angle < 45.f)
	{
		uint8 CurrentValue = Pose.GetIntValue();
		uint8 PoseSelector = CurrentValue + 3;
		if (IsValidDeathPose(PoseSelector))
		{
			DeathPose = static_cast<EDeathPose>(PoseSelector);
		}
	}
	else if (Angle >= -135.f && Angle < -45.f)
	{
		uint8 CurrentValue = Pose.GetIntValue();
		uint8 PoseSelector = CurrentValue + 6;
		if (IsValidDeathPose(PoseSelector))
		{
			DeathPose = static_cast<EDeathPose>(PoseSelector);
		}
	}
	else if (Angle >= 45.f && Angle < 135.f)
	{
		uint8 CurrentValue = Pose.GetIntValue();
		uint8 PoseSelector = CurrentValue + 8;
		if (IsValidDeathPose(PoseSelector))
		{
			DeathPose = static_cast<EDeathPose>(PoseSelector);
		}
	}
}

bool ABaseCharacter::IsValidDeathPose(uint8 Value)
{
	return Value >= static_cast<uint8>(EDeathPose::EDP_DeathForward1) && Value < static_cast<uint8>(EDeathPose::EDP_MAX);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->RecieveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(TObjectPtr<UAnimMontage> AnimMontage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayAttackMontage()
{
	AActor* WeaponOwner = HeldWeapon->GetOwner();
	if (WeaponOwner->ActorHasTag(TEXT("EngageableTarget")))
	{
		AttackMontage = HeldWeapon->GetAttackMontage();
		AttackMontageSections = HeldWeapon->GetAttackSections();
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
	}
	else if (WeaponOwner->ActorHasTag(TEXT("Enemy")))
	{
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
	}
	else
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::ClearAttackMontage()
{
	AttackMontage == nullptr;
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	
	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (HeldWeapon && HeldWeapon->GetWeaponBox())
	{
		HeldWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		HeldWeapon->IgnoreActors.Empty();
	}
}
