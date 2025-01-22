// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ShooterCharacter.h"

void AShooterAiController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!AIBehavior) return;

	RunBehaviorTree(AIBehavior);

	BlackboardComponent = GetBlackboardComponent();

	if (!BlackboardComponent) return;
	BlackboardComponent->SetValueAsVector(TEXT("OriginalLocation"), GetPawn()->GetActorLocation());
}

void AShooterAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AShooterAiController::IsDead() const
{
	AShooterCharacter* controlledCharacter = Cast<AShooterCharacter>(GetPawn());

	if (!controlledCharacter) return true;

	return controlledCharacter->IsDead();
}
