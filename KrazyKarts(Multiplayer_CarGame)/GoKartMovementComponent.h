// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
 struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;

	bool IsValid() const
	{
		return FMath::Abs(Throttle) <= 1 && FMath::Abs(SteeringThrow) <= 1;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(FGoKartMove Move);

	FVector GetCurrentVelocity();
	void SetCurrentVelocity(FVector Velocity);

	FGoKartMove GetLastMove() { return LastMove; };

	float InputThrottle;
	float InputSteeringThrow;

private:
	void CreateMove(float DeltaTime);
	void ComputeVelocity(float DeltaTime, float Throttle);
	void ApplyVelocity(float DeltaTime);
	void ComputeAngle(float DeltaTime, float SteeringThrow);
	void ApplyAngle(float DeltaTime);

	//The force apply to the car when throttle fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	UPROPERTY(EditAnywhere)
	float MaxVelocity = 10000;

	UPROPERTY(EditAnywhere)
	float DragCoeficient = 16;

	UPROPERTY(EditAnywhere)
	float RowlingCoeficient = 0.0015;

	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 50;

	//The car's mass in kg
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	FVector CurrentVelocity;
	FQuat CurrentRotation;
	FGoKartMove LastMove;
};
