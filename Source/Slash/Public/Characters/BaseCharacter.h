// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface //Multiple inheritance, a class can have more than one parent
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/* Combat */
	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	void Die(const FVector& ImpactPoint);

	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	void DirectionalDeathSelection(const FVector& ImpactPoint);
	void ChooseDeathPose(double Angle, int32 Selection);
	bool IsValidDeathPose(uint8 Value);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();
	bool IsAlive();
	void DisableMeshCollision();

	/* Montage */
	void PlayHitReactMontage(const FName& SectionName);
	void PlayDeathMontage(const FName& DeathSectionName);
	virtual int32 PlayAttackMontage();
	void StopAttackMontage();
	void ClearAttackMontage();
	virtual void PlayDodgeMontage();
	//virtual int32 PlayDeathMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> HeldWeapon; //Reference to currently held weapon
	bool bIsHoldingWeapon = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UMaterialInterface* CombatOverlay;

private:
	void PlayMontageSection(TObjectPtr<UAnimMontage> AnimMontage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Directional Death")
	TArray<FName> DeathMontageSectionsForward;

	UPROPERTY(EditAnywhere, Category = "Directional Death")
	TArray<FName> DeathMontageSectionsBack;

	UPROPERTY(EditAnywhere, Category = "Directional Death")
	TArray<FName> DeathMontageSectionsLeft;

	UPROPERTY(EditAnywhere, Category = "Directional Death")
	TArray<FName> DeathMontageSectionsRight;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
