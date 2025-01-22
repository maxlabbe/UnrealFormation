// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Mover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UMover : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMover();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Move();

private:
	UPROPERTY(EditAnywhere)
	FVector p_moveOffset;

	UPROPERTY(EditAnywhere)
	float p_moveTime = 4;

	bool p_shouldMove = false;

	FVector p_originalLocation;
	FVector p_targetLocation;

	float movementLength;
};
