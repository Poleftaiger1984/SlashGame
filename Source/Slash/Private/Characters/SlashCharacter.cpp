// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}
	
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	//Temporary if state disabling movement until I can implement attacking and running
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (GetController())
	{
		//We want to find out which way is forward. We only care for the Yaw (left and right) as our character is on the ground
		const FVector2D MovementVector = Value.Get<FVector2D>();
		const FRotator ControllerRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisValue.X);
	AddControllerPitchInput(LookAxisValue.Y);
}


void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		//Check to see the weapon type overlapped. Get Function is set up in AWeapon
		EWeaponType OverlappingWeaponType = OverlappingWeapon->GetWeaponType();
		if (!bIsHoldingWeapon && OverlappingWeaponType != EWeaponType::EWP_TwoHandedWeapon)
		{
			bIsHoldingWeapon = true;
			OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
			OverlappingWeapon->CollisionDisabler();
			//Changing OverlappingItem to null clearing the pointer so we don't interact twice
			OverlappingItem = nullptr;
			//Setting reference to weapon held
			HeldWeapon = OverlappingWeapon;

			switch (OverlappingWeaponType)
			{
			case EWeaponType::EWP_OneHandedWeapon:
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				break;

			case EWeaponType::EWP_SmallWeapon:
				CharacterState = ECharacterState::ECS_EquippedSmallWeapon;
				break;

			default:
				CharacterState = ECharacterState::ECS_Unequipped;
				break;
			}
		}
		else if (OverlappingWeaponType == EWeaponType::EWP_TwoHandedWeapon)
		{
			bIsHoldingWeapon = true;
			OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocketTwoHanded"));
			OverlappingWeapon->CollisionDisabler();
			//Changing OverlappingItem to null clearing the pointer so we don't interact twice
			OverlappingItem = nullptr;
			//Setting reference to weapon held
			HeldWeapon = OverlappingWeapon;
			switch (OverlappingWeaponType)
			{
			case EWeaponType::EWP_TwoHandedWeapon:
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				break;

			default:
				CharacterState = ECharacterState::ECS_Unequipped;
				break;
			}
		}
	}
	
}

void ASlashCharacter::Attack()
{
	//Refactoring the if conditional into CanAttack
	if (CanAttack())
	{
		//Instead of having a block of code in Attack() to handle the attack montage we refactor the attack montage handling to a new function
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;

	}
}

void ASlashCharacter::Equip()
{
	if (HeldWeapon && bIsHoldingWeapon)
	{
		EWeaponType OverlappingWeaponType = HeldWeapon->GetWeaponType();
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;

		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			switch (OverlappingWeaponType)
			{
			case EWeaponType::EWP_OneHandedWeapon:
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				break;

			case EWeaponType::EWP_SmallWeapon:
				CharacterState = ECharacterState::ECS_EquippedSmallWeapon;
				break;

			case EWeaponType::EWP_TwoHandedWeapon:
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				break;

			default:
				break;
			}
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

bool ASlashCharacter::CanAttack()
{
	return ActionState ==
		EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && HeldWeapon;
}

void ASlashCharacter::Disarm()
{
	EWeaponType HeldWeaponType = HeldWeapon->GetWeaponType();
	//Knife Weapon will be sheathed on the hip of the character therefore a different socket is required
	if (HeldWeapon && HeldWeaponType != EWeaponType::EWP_SmallWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
	else if (HeldWeapon && HeldWeaponType == EWeaponType::EWP_SmallWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName("LowerSpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	EWeaponType HeldWeaponType = HeldWeapon->GetWeaponType();
	if (HeldWeapon && HeldWeaponType != EWeaponType::EWP_TwoHandedWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
	else if (HeldWeapon && HeldWeaponType == EWeaponType::EWP_TwoHandedWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocketTwoHanded"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	AttackMontage = HeldWeapon->GetAttackMontage();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		int32 const Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	EquipMontage = HeldWeapon->GetEquipMontage();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (HeldWeapon && HeldWeapon->GetWeaponBox())
	{
		HeldWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		HeldWeapon->IgnoreActors.Empty();
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::Jump()
{
	Super::Jump();

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAround, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);

		//Equipping 
		EnhancedInputComponent->BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);

		//Equipping
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Equip);
	}

}

