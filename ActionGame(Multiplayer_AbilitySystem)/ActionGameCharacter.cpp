// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem\AbilitySystemComponents\AG_AbilitySystemComponentBase.h"
#include "AbilitySystem\AttributeSets\AG_AttributeSetBase.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "DataAssets/CharacterDataAsset.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/FootStepsComponent.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AActionGameCharacter

AActionGameCharacter::AActionGameCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	AGCharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//AbilitySystem
	AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this, &AActionGameCharacter::OnMaxMovementSpeedChanged);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));
	FootstepComponent = CreateDefaultSubobject<UFootStepsComponent>(TEXT("FootstepsComponent"));
	AGMotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	InventoryComponent->SetIsReplicated(true);
}

void AActionGameCharacter::PostLoad()
{
	Super::PostLoad();

	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AActionGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AActionGameCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) { return false; }

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return ActiveHandle.WasSuccessfullyApplied();
	}
	return false;
}

void AActionGameCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AActionGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AActionGameCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

		if (!CrouchStartEffect.Get())
		{
			return;
		}
		
		FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(CrouchStartEffect, 1, EffectContext);

		if (!EffectSpec.IsValid())
		{
			return;
		}
		
		FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

		if (!ActiveHandle.IsValid() || !ActiveHandle.WasSuccessfullyApplied())
		{
			ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect: %s"), *GetName(), *GetNameSafe(AbilitySystemComponent));
		}
	}
}

void AActionGameCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (AbilitySystemComponent && CrouchStartEffect.Get())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStartEffect, AbilitySystemComponent);
	}
}

UAG_MotionWarpingComponent* AActionGameCharacter::GetAGMotionWarpingComponent()
{
	return AGMotionWarpingComponent;
}

UInventoryComponent* AActionGameCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

void AActionGameCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AActionGameCharacter::ApplyStartUpEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
		}
	}
}

void AActionGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
	ApplyStartUpEffects();
}

void AActionGameCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AActionGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AActionGameCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

UFootStepsComponent* AActionGameCharacter::GetFootSetpComponent() const
{
	return FootstepComponent;
}

void AActionGameCharacter::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (!GetCharacterMovement()) return;

	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AActionGameCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
	
}

void AActionGameCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AActionGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AActionGameCharacter, CharacterData);
	DOREPLIFETIME(AActionGameCharacter, InventoryComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveForwardInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnMoveForwardAction);
		}

		if (MoveSideInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveSideInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnMoveSideInputAction);
		}

		if (TurnInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnTurnAction);
		}

		if (LookUpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnLookUpAction);
		}

		if (JumpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &AActionGameCharacter::OnJumpActionStarted);
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &AActionGameCharacter::OnJumpActionEnded);
		}

		if (CrouchInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &AActionGameCharacter::OnCrouchActionStarted);
			PlayerEnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Completed, this, &AActionGameCharacter::OnCrouchActionEnded);
		}

		if (SprintInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &AActionGameCharacter::OnSprintActionStarted);
			PlayerEnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AActionGameCharacter::OnSprintActionEnded);
		}

		if (DropItemInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(DropItemInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnDropItemAction);
		}

		if (EquipNextInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(EquipNextInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnEquipNextAction);
		}

		if (UnequipInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(UnequipInputAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::OnUnequipAction);
		}

		if (AttackInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &AActionGameCharacter::OnAttackActionStarted);
			PlayerEnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &AActionGameCharacter::OnAttackActionEnded);
		}

		if (AimInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Started, this, &AActionGameCharacter::OnAimActionStarted);
			PlayerEnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Completed, this, &AActionGameCharacter::OnAimActionEnded);
		}
	}
}

void AActionGameCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, MovementVector.X);
	}
}

void AActionGameCharacter::OnMoveSideInputAction(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AActionGameCharacter::OnTurnAction(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
	}
}

void AActionGameCharacter::OnLookUpAction(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerPitchInput(-LookAxisVector.X);
	}
}

void AActionGameCharacter::OnJumpActionStarted(const FInputActionValue& Value)
{
	AGCharacterMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AActionGameCharacter::OnJumpActionEnded(const FInputActionValue& Value)
{
	StopJumping();
}

void AActionGameCharacter::OnCrouchActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
	}
}

void AActionGameCharacter::OnCrouchActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void AActionGameCharacter::OnSprintActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags, true);
	}
}

void AActionGameCharacter::OnSprintActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&SprintTags);
	}
}

void AActionGameCharacter::OnDropItemAction(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::DropItemTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventPayload);
}

void AActionGameCharacter::OnEquipNextAction(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::EquipNextTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextTag, EventPayload);
}

void AActionGameCharacter::OnUnequipAction(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::UnequipTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnequipTag, EventPayload);
}

void AActionGameCharacter::OnAttackActionStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackStartEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartEventTag, EventPayload);
}

void AActionGameCharacter::OnAttackActionEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackEndEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndEventTag, EventPayload);
}

void AActionGameCharacter::OnAimActionStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AimStartEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimStartEventTag, EventPayload);
}

void AActionGameCharacter::OnAimActionEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AimEndEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimEndEventTag, EventPayload);
}
