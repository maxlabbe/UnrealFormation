// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_InventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameTypes.h"
#include "ActionGameCharacter.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ActionGameCharacter.h"
#include "Camera/CameraComponent.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InHitResult.GetActor()))
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle OutSpec = ASC->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();
	AActionGameCharacter* ActionGameCharacter = GetActionGameCharacterFromActorInfo();
	
	const FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(ActionGameCharacter, 0)->GetCameraLocation();
	const FVector CameraRotation = UGameplayStatics::GetPlayerCameraManager(ActionGameCharacter, 0)->GetCameraRotation().Vector();
	const FVector FocusTraceEnd = CameraLocation + CameraRotation * TraceDistance;
	TArray<AActor*> ActorToIgnore = { GetAvatarActorFromActorInfo() };
	FHitResult FocusHit;

	UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, FocusTraceEnd, TraceType, false, ActorToIgnore, EDrawDebugTrace::ForDuration, FocusHit, true);

	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();
	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false, ActorToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true);

	return OutHitResult.bBlockingHit;
}
