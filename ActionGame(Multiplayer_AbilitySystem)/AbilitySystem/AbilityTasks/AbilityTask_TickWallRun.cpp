// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TickWallRun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbilityTask_TickWallRun::Activate()
{
	Super::Activate();

	FHitResult OnWallHit;

	//const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if (!FindRunnableWall(OnWallHit))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();
		return;
	}

	OnWallSideDetermened.Broadcast(IsWallOnTheLeft(OnWallHit));

	CharacterOwner->Landed(OnWallHit);

	CharacterOwner->SetActorLocation(OnWallHit.ImpactPoint + OnWallHit.ImpactNormal * 60.f);

	CharacterMovement->SetMovementMode(MOVE_Flying);
}

void UAbilityTask_TickWallRun::OnDestroy(bool bInOwnerFinished)
{
	CharacterMovement->SetPlaneConstraintEnabled(false);
	CharacterMovement->SetMovementMode(MOVE_Falling);
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_TickWallRun::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FHitResult OnWallHit;

	//const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if (!FindRunnableWall(OnWallHit))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();
		return;
	}

	FRotator DirectionRotator = IsWallOnTheLeft(OnWallHit) ? FRotator(0, -90, 0) : FRotator(0, 90, 0);

	const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactNormal);

	CharacterMovement->Velocity = WallRunDirection * 700.f;

	CharacterMovement->Velocity.Z = 0;

	CharacterOwner->SetActorRotation(WallRunDirection.Rotation());

	CharacterMovement->SetPlaneConstraintEnabled(true);
	CharacterMovement->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
	CharacterMovement->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
}

bool UAbilityTask_TickWallRun::FindRunnableWall(FHitResult& OnWallHit)
{
	const FVector CharacterLocation = CharacterOwner->GetActorLocation();

	const FVector RightVector = CharacterOwner->GetActorRightVector();
	const FVector ForwardVector = CharacterOwner->GetActorForwardVector();

	const float TraceLength = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f;

	TArray<AActor*> ActorsToIgnore = { CharacterOwner };

	FHitResult TraceHit;

	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugWallRun"));
	const bool bShowTraversal = CVar->GetInt() > 0;
	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + ForwardVector * TraceLength, WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		return false;
	}

	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + -RightVector * TraceLength, WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if (FVector::DotProduct(OnWallHit.ImpactNormal, RightVector) > 0.3f)
		{
			return true;
		}
	}

	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + RightVector * TraceLength, WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if (FVector::DotProduct(OnWallHit.ImpactNormal, -RightVector) > 0.3f)
		{
			return true;
		}
	}

	return false;
}

bool UAbilityTask_TickWallRun::IsWallOnTheLeft(const FHitResult& InWallHit) const
{
	return FVector::DotProduct(CharacterOwner->GetActorRightVector(), InWallHit.ImpactNormal) > 0.f;
}

UAbilityTask_TickWallRun* UAbilityTask_TickWallRun::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner, UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectType)
{
	UAbilityTask_TickWallRun* WallRunTask = NewAbilityTask<UAbilityTask_TickWallRun>(OwningAbility);

	WallRunTask->CharacterMovement = InCharacterMovement;
	WallRunTask->CharacterOwner = InCharacterOwner;
	WallRunTask->bTickingTask = true;
	WallRunTask->WallRun_TraceObjectTypes = TraceObjectType;

	return WallRunTask;
}
