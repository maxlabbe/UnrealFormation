// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_Step.h"
#include "ActionGameCharacter.h"
#include "ActorComponents/FootStepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp);

	AActionGameCharacter* Character = Cast<AActionGameCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		if (UFootStepsComponent* FootstepsComponent = Character->GetFootSetpComponent())
		{
			FootstepsComponent->HandleFootStep(Foot);
		}

	}
}
