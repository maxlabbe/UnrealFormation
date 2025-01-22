// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...

	p_owner = GetOwner();
	p_physicsHandler = p_owner->FindComponentByClass<UPhysicsHandleComponent>();
	p_world = GetWorld();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (p_physicsHandler == nullptr || p_physicsHandler->GetGrabbedComponent() == nullptr)
		return;

	p_physicsHandler->SetTargetLocationAndRotation(GetComponentLocation() + GetForwardVector() * p_holdDistamce, GetComponentRotation());
}

void UGrabber::Grab()
{
	if (p_physicsHandler == nullptr)
		return;

	FHitResult hitResult;

	if (!TryGetGrabbableInReach(hitResult))
		return;

	UPrimitiveComponent* hitObjectComponent = hitResult.GetComponent();

	DrawDebugSphere(p_world, hitResult.Location, 10, 10, FColor::Red, false, 5);
	DrawDebugSphere(p_world, hitResult.ImpactPoint, 10, 10, FColor::Green, false, 5);

	hitObjectComponent->WakeAllRigidBodies();
	hitResult.GetActor()->Tags.Add(p_grabbedTag);
	p_physicsHandler->GrabComponentAtLocationWithRotation(hitObjectComponent, NAME_None, hitResult.ImpactPoint, hitObjectComponent->GetComponentRotation());
}

void UGrabber::Release()
{
	if (!p_physicsHandler)
		return;

	UPrimitiveComponent* component = p_physicsHandler->GetGrabbedComponent();

	if (!component)
		return;

	component->GetOwner()->Tags.Remove(p_grabbedTag);
	p_physicsHandler->ReleaseComponent();
}

bool UGrabber::TryGetGrabbableInReach(FHitResult& result) const
{
	FVector start = GetComponentLocation();
	FVector end = start + GetForwardVector() * p_maxGrabDistance;

	DrawDebugLine(p_world, start, end, FColor::Red);
	DrawDebugSphere(p_world, end, 10, 10, FColor::Blue, false, 5);
	return p_world->SweepSingleByChannel(result, start, end, FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(p_grabRadius));
}



