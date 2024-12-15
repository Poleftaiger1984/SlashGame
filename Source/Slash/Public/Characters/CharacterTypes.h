#pragma once

UENUM(BlueprintType) //BlueprintType used to expose to Blueprints
enum class ECharacterState : uint8 //uint8 lowers the size of the enum so it's more optimized
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),  //UMETA used to give fancy blueprint names
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One Handed Weapon"),
	ECS_EquippedSmallWeapon UMETA(DisplayName = "Equipped Small Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two Handed Weapon")
};