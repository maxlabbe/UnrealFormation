// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);

	// ...
}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* Owner = GetOwner(); 
	MovementComponent = Owner->FindComponentByClass<UGoKartMovementComponent>();

	if (Owner->HasAuthority())
	{
		Owner->NetUpdateFrequency = 1.f;
	}
}


// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementComponent == nullptr) return;

	APawn* Owner = Cast<APawn>(GetOwner());
	ENetRole Role = Owner->GetLocalRole();
	FGoKartMove Move = MovementComponent->GetLastMove();
	if (Role == ROLE_AutonomousProxy)
	{
		UnacknowledgeMoves.Add(Move);
		Server_SendMove(Move);
		return;
	}

	if (Owner->IsLocallyControlled())
	{
		UpdateServerState(Move);
	}
	else
	{
		ClientTick(DeltaTime);
	}
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}

void UGoKartMovementReplicator::ClearAcknowledgeMove(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		if (Move.Time > LastMove.Time)
			NewMoves.Add(Move);
	}

	UnacknowledgeMoves = NewMoves;
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
	if (MovementComponent == nullptr) return;

	AActor* Owner = GetOwner();

	ServerState.LastMove = Move;
	ServerState.Transform = Owner->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetCurrentVelocity();
}

void UGoKartMovementReplicator::ClientTick(float DeltaTime)
{
	Client_TimeSinceLastUpdate += DeltaTime;

	if (Client_TimeBetweenUpdates < SMALL_NUMBER)
		return;

	if (MovementComponent == nullptr) 
		return;

	float LerpRatio = Client_TimeSinceLastUpdate / Client_TimeBetweenUpdates;
	FHermiteCubicSpline Spline = CreateSpline();
	InterpolateLocation(Spline, LerpRatio);
	InterpolateVelocity(Spline, LerpRatio);
	InterpolateRotation(LerpRatio);
}

FHermiteCubicSpline UGoKartMovementReplicator::CreateSpline()
{
	float VelocityToDerivative = Client_TimeBetweenUpdates * 100;

	FHermiteCubicSpline Spline;
	Spline.StartLocation = Client_TransformAtLastUpdate.GetLocation();
	Spline.TargetLocation = ServerState.Transform.GetLocation();
	Spline.StartDerivative = Client_VelocityAtLastUpdate * VelocityToDerivative;
	Spline.TargetDerivative = ServerState.Velocity * VelocityToDerivative;
	return Spline;
}

void UGoKartMovementReplicator::InterpolateLocation(FHermiteCubicSpline Spline, float LerpRatio)
{
	FVector NextLocation = Spline.InterpolateLocation(LerpRatio);
	
	if (MeshOffsetRoot != nullptr) MeshOffsetRoot->SetWorldLocation(NextLocation);
}

void UGoKartMovementReplicator::InterpolateVelocity(FHermiteCubicSpline Spline, float LerpRatio)
{
	float VelocityToDerivative = Client_TimeBetweenUpdates * 100;
	FVector NewDerivative = Spline.InterpolateDerivative(LerpRatio);
	FVector NewVelocity = NewDerivative / VelocityToDerivative;
	
	MovementComponent->SetCurrentVelocity(NewVelocity);
}

void UGoKartMovementReplicator::InterpolateRotation(float LerpRatio)
{
	FQuat StartQuat = Client_TransformAtLastUpdate.Rotator().Quaternion();
	FQuat TargetQuat = ServerState.Transform.Rotator().Quaternion();
	FQuat NextQuat = FQuat::Slerp(StartQuat, TargetQuat, LerpRatio);

	if (MeshOffsetRoot != nullptr) MeshOffsetRoot->SetWorldRotation(NextQuat);
}

void UGoKartMovementReplicator::OnRep_ServerState()
{
	if (MovementComponent == nullptr) 
		return;

	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner->IsLocallyControlled())
		AutonomousProxy_OnRep_ServerState();
	else
		SimulatedProxy_OnRep_ServerState();
}

void UGoKartMovementReplicator::AutonomousProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	AActor* Owner = GetOwner();
	Owner->SetActorTransform(ServerState.Transform);
	MovementComponent->SetCurrentVelocity(ServerState.Velocity);

	ClearAcknowledgeMove(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UGoKartMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	Client_TimeBetweenUpdates = Client_TimeSinceLastUpdate;
	Client_TimeSinceLastUpdate = 0;

	if (MeshOffsetRoot != nullptr)
	{
		Client_TransformAtLastUpdate.SetLocation(MeshOffsetRoot->GetComponentLocation());
		Client_TransformAtLastUpdate.SetRotation(MeshOffsetRoot->GetComponentRotation().Quaternion());
	}

	Client_VelocityAtLastUpdate = MovementComponent->GetCurrentVelocity();

	GetOwner()->SetActorTransform(ServerState.Transform);
}

void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	ClientSimulatedTime += Move.DeltaTime;
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	float SupposedTime = ClientSimulatedTime + Move.DeltaTime;
	bool ClientNotRunningAhead = SupposedTime < GetWorld()->TimeSeconds;

	if (!ClientNotRunningAhead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client running to fast"));
		return false;
	}

	if (!Move.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid move receives"));
		return false;
	}

	return true;
}

