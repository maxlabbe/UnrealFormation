// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FootStepsComponent.h"
#include "ActionGameCharacter.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draw debug info about footsteps")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);
	

// Sets default values for this component's properties
UFootStepsComponent::UFootStepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootStepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFootStepsComponent::HandleFootStep(EFoot Foot)
{
	const int32 DebugShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();
	if (AActionGameCharacter* Character = Cast<AActionGameCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f,
				ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get()))
					{
						UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, Location, 1.f);

						if (DebugShowFootsteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White, 4.f);
						}
					}

					if (DebugShowFootsteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else
				{
					if (DebugShowFootsteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					}
				}
			}
			else
			{
				if (DebugShowFootsteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
				}
			}
		}
	}
}

