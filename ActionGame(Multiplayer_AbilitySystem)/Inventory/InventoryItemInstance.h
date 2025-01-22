// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionGameStatics.h"
#include "InventoryItemInstance.generated.h"

class AItemActor;
struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;

UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemDataStaticClass);

	virtual bool IsSupportedForNetworking() const override { return true; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;

	UFUNCTION(BlueprintPure)
	AItemActor* GetItemActor() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = "OnRep_Equipped")
	bool bIsEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnEquipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

protected:
	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;

	void TryGrantAbilities(AActor* InOwner);
	void TryRemoveAbilities(AActor* InOwner);
	void TryApplyEffects(AActor* InOwner);
	void TryRemoveEffects(AActor* InOwner);

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> OnGoingEffectHandles;
};
