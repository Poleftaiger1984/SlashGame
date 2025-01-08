// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Enemy/Enemy.h"
#include "Animation/AnimMontage.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

		//Dropping Weapon
		EnhancedInputComponent->BindAction(DropWeaponAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DropWeapon);

		//Locking onto Target
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LockOnTarget);
	}

}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	ActionState = EActionState::EAS_HitReaction;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLockedOn)
	{
		LockViewOnTarget(DeltaTime);
		CombatTargetStatus();
	}
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

	Tags.Add(FName("EngageableTarget"));
}

void ASlashCharacter::Jump()
{
	Super::Jump();
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
			OverlappingWeapon->Equip(GetMesh(), FName(WeaponSocket), this, this);
			OverlappingWeapon->SetOwner(this); //Set Weapon Actor owner 
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
			if (!HeldWeapon)
			{
				bIsHoldingWeapon = true;
				OverlappingWeapon->Equip(GetMesh(), FName(TwoHandedWeaponSocket), this, this);
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
	
}

void ASlashCharacter::Attack()
{
	Super::Attack();
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

void ASlashCharacter::DropWeapon()
{
	if (HeldWeapon && bIsHoldingWeapon == true)
	{
		UWorld* World = GetWorld();
		if (World && HeldWeapon)
		{
			HeldWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FVector PlayerLocation = GetActorLocation();
			FVector PlayerForward = GetActorForwardVector();
			float DropDistance = 75.f;
			FVector DropLocation = PlayerLocation + (PlayerForward * DropDistance);
			HeldWeapon->SetActorLocation(DropLocation);
			HeldWeapon->SetActorRotation(GetActorRotation());
			HeldWeapon->CollisionEnabler();
			HeldWeapon->SetItemState(EItemState::EIS_Hovering);
			HeldWeapon->EmbersActivate();

			HeldWeapon = nullptr;
			bIsHoldingWeapon = false;
			CharacterState = ECharacterState::ECS_Unequipped;
		}
	}
}

void ASlashCharacter::LockOnTarget(const FInputActionValue& Value)
{
	if (CombatTarget == nullptr)
	{
		FHitResult LineTraceHit;
		TargetLockTrace(LineTraceHit);

		AActor* ActorHit = LineTraceHit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Holding"));

		if (ActorHit)
		{
			bool IsActorAnEnemy = ActorHit->ActorHasTag(TEXT("Enemy"));
			if (IsActorAnEnemy)
			{
				EnemyEngaged = Cast<AEnemy>(ActorHit);
				if (EnemyEngaged)
				{
					EnemyEngaged->OverlayToggleOn();
					CombatTarget = EnemyEngaged;
					IgnoreActors.Empty();
					IsLockedOn = true;
				}
			}
		}
	}
	else if (CombatTarget != nullptr)
	{
		CombatTarget = nullptr;
		IsLockedOn = false;
		EnemyEngaged = nullptr;
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
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

void ASlashCharacter::TargetLockTrace(FHitResult& LineTraceHit)
{
	const FVector Start = ViewCamera->GetComponentLocation();
	const FVector End = Start + (ViewCamera->GetForwardVector() * LockOnRange);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	UKismetSystemLibrary::LineTraceSingle(this, Start, End, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowLineDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, LineTraceHit, true);
	IgnoreActors.AddUnique(LineTraceHit.GetActor());
	
}

void ASlashCharacter::CombatTargetStatus()
{
	if (EnemyEngaged)
	{
		EEnemyState EngagedEnemyState = EnemyEngaged->GetEnemyState();
		if (EngagedEnemyState == EEnemyState::EES_Dead)
		{
			CombatTarget = nullptr;
			EnemyEngaged = nullptr;
			IsLockedOn = false;
			UE_LOG(LogTemp, Warning, TEXT("Combat Target Dead"));
		}
	}
}

void ASlashCharacter::LockViewOnTarget(float DeltaTime)
{
	//if (EnemyEngaged && ViewCamera)
	//{
		const FRotator CameraRotation = GetControlRotation();
		const FRotator EnemyRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyEngaged->GetActorLocation());

	//	FVector CameraVector = CameraRotation.Vector();
	//	CameraVector.Normalize();
	//	FVector EnemyVector = EnemyRotation.Vector();
	//	EnemyVector.Normalize();

	//	float DotProduct = FVector::DotProduct(CameraVector, EnemyVector);

	//	//Change angle to radians
	//	float AngleInRadians = FMath::Acos(DotProduct);
	//	//Change angle to degrees
	//	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	//	if (AngleInDegrees > LockOnFieldOfView)
	//	{
	//		IsAdjustingCamera = true;
	//		StartCameraAdjustingTimer();
	//	}
	//	if (IsAdjustingCamera)
	//	{
			const FRotator CameraRotator = FRotator(CameraRotation.Pitch, EnemyRotation.Yaw, CameraRotation.Roll);
			const FRotator FinalRotator = UKismetMathLibrary::RInterpTo(CameraRotation, CameraRotator, DeltaTime, ViewInterpolationSpeed);
			Controller->SetControlRotation(FinalRotator);
			UE_LOG(LogTemp, Warning, TEXT("Adjusting Camera"));
		//}

	//}
}

void ASlashCharacter::StartCameraAdjustingTimer()
{
	GetWorldTimerManager().SetTimer(AdjustCameraTimer, this, &ASlashCharacter::AdjustCamera, CameraAdjustingTime);
}

void ASlashCharacter::AdjustCamera()
{
	IsAdjustingCamera = false;
	GetWorldTimerManager().ClearTimer(AdjustCameraTimer);
}


void ASlashCharacter::AttachWeaponToBack()
{
	EWeaponType HeldWeaponType = HeldWeapon->GetWeaponType();
	//Knife Weapon will be sheathed on the hip of the character therefore a different socket is required
	if (HeldWeapon && HeldWeaponType != EWeaponType::EWP_SmallWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName(WeaponSheathSocket));
	}
	else if (HeldWeapon && HeldWeaponType == EWeaponType::EWP_SmallWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName(SmallWeaponSheathSocket));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	EWeaponType HeldWeaponType = HeldWeapon->GetWeaponType();
	if (HeldWeapon && HeldWeaponType != EWeaponType::EWP_TwoHandedWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName(WeaponSocket));
	}
	else if (HeldWeapon && HeldWeaponType == EWeaponType::EWP_TwoHandedWeapon)
	{
		HeldWeapon->AttachMeshToSocket(GetMesh(), FName(TwoHandedWeaponSocket));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

