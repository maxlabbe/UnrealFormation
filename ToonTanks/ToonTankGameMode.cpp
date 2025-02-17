// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTankGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTankPlayerController.h"

void AToonTankGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Tank)
	{
		if (!ToonTankPlayerController)
			return;

		Tank->HandleDestruction();
		ToonTankPlayerController->SetPlayerEnabledState(false);

		for (AActor* tower : Towers)
		{
			Cast<ATower>(tower)->bIsEnabled = false;
		}

		GameOver(false);
	}

	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))
	{
		DestroyedTower->HandleDestruction();
		TargetTowers--;

		if (TargetTowers != 0)
			return;

		ToonTankPlayerController->SetPlayerEnabledState(false);

		for (AActor* tower : Towers)
		{
			Cast<ATower>(tower)->bIsEnabled = false;
		}

		GameOver(true);
	}
}

void AToonTankGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

void AToonTankGameMode::HandleGameStart()
{
	GetTargetTowersCount();

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	
	if (!Tank)
		return;

	ToonTankPlayerController = Cast<AToonTankPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (!ToonTankPlayerController)
		return;

	StartGame();

	ToonTankPlayerController->SetPlayerEnabledState(false);

	FTimerHandle PlayerEnbaleTimerHandle;
	FTimerDelegate PlayerEnbaleTimerDelegate = FTimerDelegate::CreateUObject(ToonTankPlayerController, &AToonTankPlayerController::SetPlayerEnabledState, true);
	GetWorldTimerManager().SetTimer(PlayerEnbaleTimerHandle, PlayerEnbaleTimerDelegate, StartDelay, false);
}

void AToonTankGameMode::GetTargetTowersCount()
{
	UGameplayStatics::GetAllActorsOfClass(this, TowerClass, Towers);
	TargetTowers = Towers.Num();

}
