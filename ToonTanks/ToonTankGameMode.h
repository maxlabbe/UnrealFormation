// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTankGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API AToonTankGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	void ActorDied(AActor* DeadActor);


protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);

private:

	class ATank* Tank;
	class AToonTankPlayerController* ToonTankPlayerController;

	UPROPERTY(EditAnywhere)
	float StartDelay = 3.0f;
	UPROPERTY(EditAnywhere)
	class TSubclassOf<class ATower> TowerClass;
	TArray<AActor*> Towers;
	int32 TargetTowers = 0;

	void HandleGameStart();
	void GetTargetTowersCount();
};
