// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "GA_InventoryAbility.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UItemStaticData;
class UWeaponStaticData;
class AItemActor;
class AWeaponItemActor;

UCLASS()
class ACTIONGAME_API UGA_InventoryAbility : public UAG_GameplayAbility
{
	GENERATED_BODY()
	

public :
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	/*virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	*/

	UFUNCTION(BlueprintPure)
	const UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance() const;

	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData() const;

	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedItemWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	AItemActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor() const;

protected:
	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* InventoryComponent = nullptr;
};
