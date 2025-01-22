// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "ActionGameStatics.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystemLog.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemDataStaticClass)
{
	ItemStaticDataClass = InItemDataStaticClass;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bIsEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UActionGameStatics::GetItemStaticData(ItemStaticDataClass);
}

AItemActor* UInventoryItemInstance::GetItemActor() const
{
	return ItemActor;
}

void UInventoryItemInstance::OnRep_Equipped()
{
	
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if (UWorld* World = InOwner->GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();
		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActor>(StaticData->ItemActorClass, Transform, InOwner, Cast<APawn>(InOwner), 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, ESpawnActorScaleMethod::SelectDefaultAtRuntime);
		ItemActor->Init(this);
		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);

		if (USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
		{
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
		}
	}

	TryGrantAbilities(InOwner);
	TryApplyEffects(InOwner);

	bIsEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped(AActor* InOwner)
{
	if (IsValid(ItemActor))
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	bIsEquipped = false;
	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if (IsValid(ItemActor))
	{
		ItemActor->OnDropped();
	}

	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);

	bIsEquipped = false;
}

void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority())
	{
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for (auto ItemAbility : StaticData->GrantedAbilities)
			{
				GrantedAbilityHandles.Add(AbilityComponent->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority())
	{
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for (auto AbilityHandle : GrantedAbilityHandles)
			{
				AbilityComponent->ClearAbility(AbilityHandle);
			}

			GrantedAbilityHandles.Empty();
		}
	}
}

void UInventoryItemInstance::TryApplyEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		const UItemStaticData* ItemStaticData = GetItemStaticData();
		FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

		for (auto GameplayEffect : ItemStaticData->OnGoingEffects)
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
					OnGoingEffectHandles.Add(ActiveHandle);
				}
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		for (FActiveGameplayEffectHandle ActiveEffectHandle : OnGoingEffectHandles)
		{
			if (ActiveEffectHandle.IsValid())
			{
				AbilityComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
		}

		OnGoingEffectHandles.Empty();
	}
}
