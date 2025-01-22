// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API AWeaponItemActor : public AItemActor
{
	GENERATED_BODY()
	
	AWeaponItemActor();
	
	const UWeaponStaticData* GetWeaponStaticData() const;

public:

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable)
	void PlayerWeaponEffects(const FHitResult& InHitResult);

protected:

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastPlayerWeaponEffects(const FHitResult& InHitResult);

	void PlayerWeaponEffectsInternal(const FHitResult& InHitResult);

	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;
};
