#pragma once

UENUM(BlueprintType) //BlueprintType used to expose to Blueprints
enum class ECharacterState : uint8 //uint8 lowers the size of the enum so it's more optimized
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),  //UMETA used to give fancy blueprint names
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One Handed Weapon"),
	ECS_EquippedSmallWeapon UMETA(DisplayName = "Equipped Small Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWP_Unknown UMETA(DisplayName = "Unknown"),
	EWP_OneHandedWeapon UMETA(DisplayName = "One Handed Weapon"),
	EWP_SmallWeapon UMETA(DisplayName = "Small Weapon"),
	EWP_TwoHandedWeapon UMETA(DisplayName = "Two Handed Weapon")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),
};