// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PullTrigger();
	void ReleaseTrigger();

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

private:
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);

	void GamepadLookUp(float axisValue);
	void GamepadLookRight(float axisValue);

	UPROPERTY(EditAnywhere)
	float RotationRate = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<class AGun> GunClass;

	UPROPERTY(EditDefaultsOnly)
	float MaxLifeAmount;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentLifeAmount;

	AGun* Gun;
};
