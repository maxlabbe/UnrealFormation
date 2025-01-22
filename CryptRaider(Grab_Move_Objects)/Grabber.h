// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Release();

private :

	bool TryGetGrabbableInReach(FHitResult& result) const;

	UPROPERTY(EditAnywhere)
	float p_maxGrabDistance = 400;

	UPROPERTY(EditAnywhere)
	float p_grabRadius = 100;

	UPROPERTY(EditAnywhere)
	float p_holdDistamce = 100;
		
	UPROPERTY(EditAnywhere)
	FName p_grabbedTag;

	AActor* p_owner;
	UWorld* p_world;
	UPhysicsHandleComponent* p_physicsHandler;
};
