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

void ABaseCharacter::Attack()
{

}

void ABaseCharacter::Die(const FVector& ImpactPoint)
{
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

	PlayDeathMontage(DeathSectionName);
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

//int32 ABaseCharacter::PlayDeathMontage()
//{
//	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
//}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayAttackMontage()
{
	if (AttackMontage == nullptr)
	{
		AttackMontage = HeldWeapon->GetAttackMontage();
		AttackMontageSections = HeldWeapon->GetAttackSections();
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
	}
	else 
	{
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
	}
	
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
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
