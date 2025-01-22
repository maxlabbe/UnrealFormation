// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"
#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* Owner = Cast<APawn>(GetOwner());
	ENetRole Role = Owner->GetLocalRole();

	if (Owner->IsLocallyControlled())
	{
		CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
}

void UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	LastMove.DeltaTime = DeltaTime;
	LastMove.SteeringThrow = InputSteeringThrow;
	LastMove.Throttle = InputThrottle;
	LastMove.Time = GetOwner()->GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void UGoKartMovementComponent::SimulateMove(FGoKartMove Move)
{
	ComputeAngle(Move.DeltaTime, Move.SteeringThrow);
	ComputeVelocity(Move.DeltaTime, Move.Throttle);
	ApplyAngle(Move.DeltaTime);
	ApplyVelocity(Move.DeltaTime);
}

void UGoKartMovementComponent::ComputeAngle(float DeltaTime, float SteeringThrow)
{
	AActor* Owner = GetOwner();
	float Rotation = FVector::DotProduct(Owner->GetActorForwardVector(), CurrentVelocity) * DeltaTime / MinTurningRadius * SteeringThrow;
	CurrentRotation = FQuat(Owner->GetActorUpVector(), Rotation);

	CurrentVelocity = CurrentRotation.RotateVector(CurrentVelocity);
}

void UGoKartMovementComponent::ComputeVelocity(float DeltaTime, float Throttle)
{
	AActor* Owner = GetOwner();
	FVector EngineForce = Owner->GetActorForwardVector() * MaxDrivingForce * Throttle;
	FVector InverseVelocity = -1 * CurrentVelocity.GetSafeNormal();
	FVector Drag = InverseVelocity * CurrentVelocity.SizeSquared() * DragCoeficient;
	FVector Rowling = InverseVelocity * Mass * -GetWorld()->GetGravityZ() * RowlingCoeficient;
	FVector SumForces = EngineForce + Drag + Rowling;
	FVector Acceleration = (SumForces) / Mass;

	CurrentVelocity += Acceleration * DeltaTime;
}

void UGoKartMovementComponent::ApplyAngle(float DeltaTime)
{
	AActor* Owner = GetOwner();
	Owner->AddActorWorldRotation(CurrentRotation);
}

void UGoKartMovementComponent::ApplyVelocity(float DeltaTime)
{
	AActor* Owner = GetOwner();
	FVector Translation = CurrentVelocity * DeltaTime;

	FHitResult hitResult;
	Owner->AddActorWorldOffset(CurrentVelocity, true, &hitResult);

	if (hitResult.IsValidBlockingHit())
		CurrentVelocity *= 0;
}

FVector UGoKartMovementComponent::GetCurrentVelocity()
{
	return CurrentVelocity;
}

void UGoKartMovementComponent::SetCurrentVelocity(FVector Velocity)
{
	CurrentVelocity = Velocity;
}
