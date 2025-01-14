// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PickupInterface.h"

// Add default functionality here for any IPickupInterface functions that are not pure virtual.

void IPickupInterface::SetOverlappingItem(TObjectPtr<class AItem> Item)
{
}

void IPickupInterface::AddSouls(TObjectPtr<class ASoul> Soul)
{
}

void IPickupInterface::AddGold(TObjectPtr<class ATreasure> Treasure)
{
}

void IPickupInterface::AddHealthBoost(TObjectPtr<class APotion> Potion)
{
}
