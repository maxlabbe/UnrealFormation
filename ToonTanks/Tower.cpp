// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsEnabled || !Tank)
		return;

	if(TankIsInRange())
		RotateTurret(Tank->GetActorLocation());
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);

	bIsEnabled = true;
}

void ATower::CheckFireCondition()
{
	if (bIsEnabled && TankIsInRange())
		Fire();
}

bool ATower::TankIsInRange()
{
	FVector tankLocation = Tank->GetActorLocation();
	return FVector::Dist(GetActorLocation(), tankLocation) <= Range;

}