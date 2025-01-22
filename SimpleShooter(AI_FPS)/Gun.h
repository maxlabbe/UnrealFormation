// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class SIMPLESHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void PullTrigger();
	void ReleaseTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* MuzzleParticle;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	class USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	bool bIsAutoOrMultipleRound;

	UPROPERTY(EditAnywhere)
	int BulletPerShot;

	UPROPERTY(EditAnywhere)
	float FireRate;

	UPROPERTY(EditAnywhere)
	float Range;

	UPROPERTY(EditAnywhere)
	float DamagePerBullet = 40;

	int CurrentBulletsNumber;
	FTimerHandle TimerHandler;

	AController* Controller;

	void Shoot();
	void ResetShoot();

	AController* GetOwnerController() const;
	bool BulletTrace(FHitResult& HitResult, FVector& ShotDirection);

};
