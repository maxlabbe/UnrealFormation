// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "ShooterAiController.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
	NodeName = TEXT("Update Player Location if seen");
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AShooterAiController* AiController = Cast<AShooterAiController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!AiController || !BlackboardComponent || !PlayerPawn) return;

	if (AiController->LineOfSightTo(PlayerPawn))
		BlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
	else
		BlackboardComponent->ClearValue(GetSelectedBlackboardKey());
}
