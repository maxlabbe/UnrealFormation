// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "ActionGameCharacter.h"
#include "AbilitySystemLog.h"

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
	{

		FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

		for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
		{
			if (!GameplayEffect.Get()) continue;

			FGameplayEffectSpecHandle EffectSpec = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

			if (EffectSpec.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

				if (!ActiveHandle.IsValid() || !ActiveHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect: %s"), *GetName(), *GetNameSafe(GameplayEffect));
				}
			}
		}

		if (IsInstantiated())
		{
			for (auto GameplayEffect : OngoingEffectsToRemovedOnEnd)
			{
				if (!GameplayEffect.Get()) continue;

				FGameplayEffectSpecHandle EffectSpec = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

				if (EffectSpec.IsValid())
				{
					FActiveGameplayEffectHandle ActiveHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

					if (!ActiveHandle.IsValid() || !ActiveHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect: %s"), *GetName(), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveHandle);
					}

				}
			}
		}
	}
}

void UAG_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated())
	{
		if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			for (FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandles)
			{
				if (ActiveEffectHandle.IsValid())
				{
					AbilityComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
				}
			}

			RemoveOnEndEffectHandles.Empty();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AActionGameCharacter* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
	return Cast<AActionGameCharacter>(GetAvatarActorFromActorInfo());
}
