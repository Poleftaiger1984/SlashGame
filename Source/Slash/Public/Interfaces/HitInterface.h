// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent) //BlueprintNativeEvent makes it so the Function is overwrittable everywhere, both in BP and in C++. Function now requires no virtual or =0
	void GetHit(const FVector& ImpactPoint, AActor* Hitter); //Pure Virtual Function. Makes the class abstract and the function requires no definition when it =0
};
