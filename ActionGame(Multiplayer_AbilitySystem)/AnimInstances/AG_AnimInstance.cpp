// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "ActionGameTypes.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpace.h"
#include "Inventory/InventoryItemInstance.h"
#include "ActorComponents/InventoryComponent.h"
#include "DataAssets/CharacterAnimDataAsset.h"

const UItemStaticData* UAG_AnimInstance::GetEquippedItemData() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	UInventoryComponent* InventoryComponent = ActionGameCharacter ? ActionGameCharacter->GetInventoryComponent() : nullptr;
	UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;

	return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
	if (AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if (ItemData->CharacterAnimationData.MovementBlendSpace)
			{
				return ItemData->CharacterAnimationData.MovementBlendSpace;
			}
		}

		FCharacterData Data = ActionGameCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
	if (AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if (ItemData->CharacterAnimationData.IdleAnimationAsset)
			{
				return ItemData->CharacterAnimationData.IdleAnimationAsset;
			}
		}

		FCharacterData Data = ActionGameCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if (AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if (ItemData->CharacterAnimationData.CrouchMovementBlendSpace)
			{
				return ItemData->CharacterAnimationData.CrouchMovementBlendSpace;
			}
		}

		FCharacterData Data = ActionGameCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
	if (AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if (ItemData->CharacterAnimationData.CrouchIdleAnimationAsset)
			{
				return ItemData->CharacterAnimationData.CrouchIdleAnimationAsset;
			}
		}

		FCharacterData Data = ActionGameCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
