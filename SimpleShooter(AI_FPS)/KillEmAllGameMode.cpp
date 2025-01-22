// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEmAllGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AiController.h"
#include "GameFramework/Controller.h"
#include "ShooterAiController.h"

void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());

	if (PlayerController)
	{
		EndGame(false);
		return;
	}

	for (AShooterAiController* Controller : TActorRange<AShooterAiController>(GetWorld()))
	{
		if (!Controller->IsDead()) return;
	}

	EndGame(true);
}

void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		Controller->GameHasEnded(Controller->GetPawn(), Controller->IsPlayerController() == bIsPlayerWinner);
	}


}
