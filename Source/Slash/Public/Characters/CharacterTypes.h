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
	EDP_DeathBack1 UMETA(DisplayName = "DeathBack1"),
	EDP_DeathBack2 UMETA(DisplayName = "DeathBack2"),
	EDP_DeathBack3 UMETA(DisplayName = "DeathBack3"),
	EDP_DeathForward1 UMETA(DisplayName = "DeathForward1"),
	EDP_DeathForward2 UMETA(DisplayName = "DeathForward2"),
	EDP_DeathForward3 UMETA(DisplayName = "DeathForward3"),
	EDP_DeathLeft1 UMETA(DisplayName = "DeathLeft1"),
	EDP_DeathLeft2 UMETA(DisplayName = "DeathLeft2"),
	EDP_DeathRight1 UMETA(DisplayName = "DeathRight1"),
	EDP_DeathRight2 UMETA(DisplayName = "DeathRight2")
};