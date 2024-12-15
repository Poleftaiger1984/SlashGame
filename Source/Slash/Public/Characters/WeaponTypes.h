#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWP_Unknown UMETA(DisplayName = "Unknown"),
	EWP_OneHandedWeapon UMETA(DisplayName = "One Handed Weapon"),
	EWP_SmallWeapon UMETA(DisplayName = "Small Weapon"),
	EWP_TwoHandedWeapon UMETA(DisplayName = "Two Handed Weapon")
};



