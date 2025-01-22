// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASimpleShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);


	HudScreen->RemoveFromViewport();

	UUserWidget* EndScreen;

	if (bIsWinner)
	{
		EndScreen = CreateWidget(this, WinPanelClass);
	}
	else
	{
		EndScreen = CreateWidget(this, LosePanelClass);
	}
	 
	if (!EndScreen) return;

	EndScreen->AddToViewport();

	GetWorldTimerManager().SetTimer(RestartTimer, [this]()
		{
			UGameplayStatics::GetPlayerController(this, 0)->RestartLevel();
			HudScreen->AddToViewport();
		}, RestartDelay, false);
}

void ASimpleShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HudScreen = CreateWidget(this, HUDClass);
	HudScreen->AddToViewport();
}
