// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GameplayEffectTypes.h"
#include "ActionGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAG_AbilitySystemComponentBase;
class UAbilitySystemComponent;
class UAG_AttributeSetBase;
class UGameplayEffect;
class UGameplayAbility;
class UAG_MotionWarpingComponent;
class UAG_CharacterMovementComponent;
class UInventoryComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AActionGameCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:

	AActionGameCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual void PawnClientRestart() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UAG_MotionWarpingComponent* GetAGMotionWarpingComponent();

	UInventoryComponent* GetInventoryComponent() const;

protected:

	void GiveAbilities();
	void ApplyStartUpEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly)
	UAG_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAG_AttributeSetBase* AttributeSet;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
	UAG_MotionWarpingComponent* AGMotionWarpingComponent;

	UAG_CharacterMovementComponent* AGCharacterMovementComponent;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const { return CharacterData; }

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	class UFootStepsComponent* GetFootSetpComponent() const;

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	UPROPERTY(EditAnywhere)
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	UFootStepsComponent* FootstepComponent;

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveForwardInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveSideInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* TurnInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookUpInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* CrouchInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* SprintInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* DropItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* EquipNextInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* UnequipInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* AttackInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* AimInputAction;

	void OnMoveForwardAction(const FInputActionValue& Value);
	void OnMoveSideInputAction(const FInputActionValue& Value);
	void OnTurnAction(const FInputActionValue& Value);
	void OnLookUpAction(const FInputActionValue& Value);
	void OnJumpActionStarted(const FInputActionValue& Value);
	void OnJumpActionEnded(const FInputActionValue& Value);
	void OnCrouchActionStarted(const FInputActionValue& Value);
	void OnCrouchActionEnded(const FInputActionValue& Value);
	void OnSprintActionStarted(const FInputActionValue& Value);
	void OnSprintActionEnded(const FInputActionValue& Value);
	void OnDropItemAction(const FInputActionValue& Value);
	void OnEquipNextAction(const FInputActionValue& Value);
	void OnUnequipAction(const FInputActionValue& Value);
	void OnAttackActionStarted(const FInputActionValue& Value);
	void OnAttackActionEnded(const FInputActionValue& Value);
	void OnAimActionStarted(const FInputActionValue& Value);
	void OnAimActionEnded(const FInputActionValue& Value);
	

protected:
	//Gameplay Events
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackStartEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackEndEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimStartEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimEndEventTag;

	//Gameplay Tags
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	//GameplayE Effects
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStartEffect;

	//Delegates
	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;

	//Inventory
	UPROPERTY(EditAnywhere, Replicated)
	UInventoryComponent* InventoryComponent = nullptr;
};

