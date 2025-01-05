// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h" 
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;
	virtual void Jump() override;

	/* Callbacks for input  */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Equip();
	void DropWeapon();

	/* Combat */
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void PlayEquipMontage(const FName& SectionName);
	

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

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

private:
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

public:
	/*Inline functions are more optimized as compilers take the code as is and paste it wherever the function is called
	* This should be done only for small functions like getters and setters. FORCEINLINE is an UE Macro that forces
	* the compiler to keep the function inline */
	FORCEINLINE void SetOverlappingItem(TObjectPtr<AItem> Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};
