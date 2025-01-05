// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EWeaponType: uint8;
class UAnimMontage;
class USoundBase;
class UBoxComponent;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(TObjectPtr<USceneComponent> InParent, FName InSocketName, TObjectPtr<AActor> NewOwner, TObjectPtr<APawn> NewInstigator);

	void DeactivateEmbers();

	void PlayEquipSound();

	void AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName InSocketName);

	//void IsAttachedToPlayer(TObjectPtr<AActor> Parent, TObjectPtr<AItem> ItemBeingChecked);
	
	virtual EWeaponType GetWeaponType();

	void SetWeaponType(EWeaponType NewWeaponType);

	TObjectPtr<UAnimMontage> GetAttackMontage() const;
	TObjectPtr<UAnimMontage> GetEquipMontage() const;
	TArray<FName> GetAttackSections() const;
	void CollisionDisabler();
	void CollisionEnabler();
	void EmbersActivate();

	TArray<TObjectPtr<AActor>> IgnoreActors;


protected:
	virtual void BeginPlay() override;
	
	EWeaponType WeaponClass;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	FVector BoxTraceExtent = FVector(8.f);
	
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	//Start and end for box trace
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBox; }
};
