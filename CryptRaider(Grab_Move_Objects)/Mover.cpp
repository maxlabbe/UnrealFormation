// Fill out your copyright notice in the Description page of Project Settings.


#include "Mover.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMover::UMover()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMover::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("MOVER : %u"), GetOwner());
	// ...

	p_originalLocation = GetOwner()->GetActorLocation();
	p_targetLocation = p_originalLocation + p_moveOffset;
	movementLength = p_moveOffset.Length();
}


// Called every frame
void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!p_shouldMove)
		return;

	AActor* owner = GetOwner();
	FVector currentLocation = owner->GetActorLocation();
	float speed = movementLength / p_moveTime;

	owner->SetActorLocation(FMath::VInterpConstantTo(currentLocation, p_targetLocation, DeltaTime, speed));

	p_shouldMove = currentLocation == p_targetLocation;
}

void UMover::Move()
{
	FTimerHandle timerHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(timerHandle, [this]()
	{
		p_shouldMove = true;
	}, 1.5, false);
}

