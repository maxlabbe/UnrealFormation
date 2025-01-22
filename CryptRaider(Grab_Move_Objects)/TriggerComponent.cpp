// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<AActor*> actors;

	GetOverlappingActors(actors);

	if (actors.Num() == 0)
		return;

	for (AActor* actor : actors)
	{
		if (!actor->ActorHasTag(p_triggerTag) || actor->ActorHasTag(p_grabbedTag))
			continue;

		UPrimitiveComponent* component = Cast<UPrimitiveComponent>(actor->GetRootComponent());

		if (component != nullptr)
			component->SetSimulatePhysics(false);

		actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		actor->SetActorLocationAndRotation(p_statueAncor->GetComponentLocation(), p_statueAncor->GetComponentRotation());
		p_mover->Move();
		this->Activate(false);
	}
}

void UTriggerComponent::SetMover(UMover* mover)
{
	p_mover = mover;
}

void UTriggerComponent::SetStatueAncor(UChildActorComponent* statueAncor)
{
	p_statueAncor = statueAncor;
}