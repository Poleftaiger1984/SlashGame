// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EWeaponType: uint8;
class UAnimMontage;
class USoundBase;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	void Equip(TObjectPtr<USceneComponent> InParent, FName InSocketName);

	void AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName InSocketName);

	//void IsAttachedToPlayer(TObjectPtr<AActor> Parent, TObjectPtr<AItem> ItemBeingChecked);
	
	virtual EWeaponType GetWeaponType();

	void SetWeaponType(EWeaponType NewWeaponType);

	TObjectPtr<UAnimMontage> GetAttackMontage() const;
	TObjectPtr<UAnimMontage> GetEquipMontage() const;
	void CollisionDisabler();

protected:
	
	EWeaponType WeaponClass;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

private:

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;





	

	
	

};
