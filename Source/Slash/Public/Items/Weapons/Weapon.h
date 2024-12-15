// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EWeaponType: uint8;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	void Equip(TObjectPtr<USceneComponent> InParent, FName InSocketName);

	//void IsAttachedToPlayer(TObjectPtr<AActor> Parent, TObjectPtr<AItem> ItemBeingChecked);
	
	virtual EWeaponType GetWeaponType();

	void SetWeaponType(EWeaponType NewWeaponType);

protected:
	
	EWeaponType WeaponClass;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;




	

	
	

};
