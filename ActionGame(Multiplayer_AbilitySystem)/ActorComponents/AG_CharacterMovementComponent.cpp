// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draw info about traversal")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

static TAutoConsoleVariable<int32> CVarShowWallRun(
	TEXT("ShowDebugWallRun"),
	0,
	TEXT("Draw info about wall run")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

bool UAG_CharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		if (!AbilityClass.Get()) continue;

		if (!ASC->TryActivateAbilityByClass(AbilityClass, true)) continue;

		FGameplayAbilitySpec* Spec;
		Spec = ASC->FindAbilitySpecFromClass(AbilityClass);

		if (Spec && Spec->IsActive()) return true;
	}
	return false;
}

void UAG_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	HandleMovementDirection();

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe"), EGameplayTagEventType::NewOrRemoved)).AddUObject(this, &UAG_CharacterMovementComponent::OnEnforcedStrafeTagChanged);
	}
}

EMovementDirectionType UAG_CharacterMovementComponent::GetMovementDirectionType()
{
	return MovementDirectionType;
}

void UAG_CharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirtectionType)
{
	MovementDirectionType = InMovementDirtectionType;
	HandleMovementDirection();
}

void UAG_CharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}

void UAG_CharacterMovementComponent::HandleMovementDirection()
{
	switch (MovementDirectionType)
	{
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;
	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
	}
}
