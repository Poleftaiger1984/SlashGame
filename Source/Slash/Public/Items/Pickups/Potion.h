// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/PickupInterface.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API APotion : public AItem
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float HealthBoost;

public:
	FORCEINLINE float GetHealthBoost() const { return HealthBoost; }
};
