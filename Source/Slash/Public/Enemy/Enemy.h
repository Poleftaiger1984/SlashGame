// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface //Multiple inheritance, a class can have more than one parent
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override; 
	//_Implementation is the overridable BlueprintNativeEvent function automatically created when we declared it in the HitInterface
	void DirectionalHitReact(const FVector& ImpactPoint);
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


protected:
	virtual void BeginPlay() override;

	void Die(const FVector& ImpactPoint);
	FName DirectionalDeathSelection(const FVector& ImpactPoint, FName& DeathSectionName);
	bool InTargetRange(TObjectPtr<AActor> Target, double Radius);
	void MoveToTarget(TObjectPtr<AActor> Target);
	TObjectPtr<AActor> ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	/*
	* Play Montage Functions
	*/
	void PlayHitReactMontage(const FName& SectionName);
	void PlayDeathMontage(const FName& DeathSectionName);


	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


private:

	/*
	* Components
	*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	/*
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/*
	* Navigation
	*/

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

public:

};
