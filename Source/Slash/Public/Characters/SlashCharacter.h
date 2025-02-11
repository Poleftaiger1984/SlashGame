// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h" 
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AEnemy;
class ASoul;
class ATreasure;
class USlashOverlay;
class APotion;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(TObjectPtr<AItem> Item) override;
	virtual void AddSouls(TObjectPtr<ASoul> Soul) override;
	virtual void AddGold(TObjectPtr<ATreasure> Treasure) override;
	virtual void AddHealthBoost(TObjectPtr<APotion> Potion) override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/* Callbacks for input  */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Equip();
	void DropWeapon();
	void LockOnTarget();
	void Dodge();

	/* Combat */
	bool HasEnoughStamina();
	bool IsOccupied() const;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void PlayEquipMontage(const FName& SectionName);
	void TargetLockTrace(FHitResult& LineTraceHit);	
	void CombatTargetStatus();
	void LockViewOnTarget(float DeltaTime);
	void StartCameraAdjustingTimer();
	void AdjustCamera();
	virtual void Die_Implementation(const FVector& ImpactPoint) override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> SlashContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EKeyPressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DropWeaponAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LockOnTargetAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LockOnRange = 100.f;

	TArray<TObjectPtr<AActor>> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bShowLineDebug = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AEnemy> EnemyEngaged; //Reference to enemy actively engaged in combat with

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsCombatTargetAlive = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsLockedOn = false;

	bool bIsAdjustingCamera = false;
	FTimerHandle AdjustCameraTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LockOnFieldOfView = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CameraAdjustingTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ViewInterpolationSpeed = 10.f;

private:
	void InitializeHUDOverlay();
	void SetHUDHealth();

	/* Character Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName TwoHandedWeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSheathSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName SmallWeaponSheathSocket;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	TObjectPtr<USlashOverlay> SlashOverlay;

public:
	/*Inline functions are more optimized as compilers take the code as is and paste it wherever the function is called
	* This should be done only for small functions like getters and setters. FORCEINLINE is an UE Macro that forces
	* the compiler to keep the function inline */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};
