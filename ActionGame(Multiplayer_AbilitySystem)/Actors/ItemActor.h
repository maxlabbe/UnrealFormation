// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;

UCLASS()
class ACTIONGAME_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	virtual void OnEquipped();
	virtual void OnUnEquipped();
	virtual void OnDropped();

	virtual bool ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void Init(UInventoryItemInstance* Instance);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = "OnRep_ItemInstance")
	UInventoryItemInstance* ItemInstance = nullptr;

	UPROPERTY(ReplicatedUsing = "OnRep_ItemState")
	EItemState ItemState = EItemState::None;
	
	UFUNCTION()
	void OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance);
	
	UFUNCTION()
	void OnRep_ItemState();

	UPROPERTY()
	class USphereComponent* SphereComponent = nullptr;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	virtual void InitInternal();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
