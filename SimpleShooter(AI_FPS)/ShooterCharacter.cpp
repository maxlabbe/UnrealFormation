// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	damageApplied = FMath::Min(CurrentLifeAmount, damageApplied);
	CurrentLifeAmount -= damageApplied;

	if (IsDead())
	{
		ASimpleShooterGameModeBase* GameMode = Cast<ASimpleShooterGameModeBase>(UGameplayStatics::GetGameMode(this));

		if (GameMode)
		{
			GameMode->PawnKilled(this);
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
	}

	return damageApplied;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentLifeAmount = MaxLifeAmount;
	
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("GamepadLookUp"), this, &AShooterCharacter::GamepadLookUp);
	PlayerInputComponent->BindAxis(TEXT("GamepadLookRight"), this, &AShooterCharacter::GamepadLookRight);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &AShooterCharacter::PullTrigger);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Released, this, &AShooterCharacter::ReleaseTrigger);
}



bool AShooterCharacter::IsDead() const
{
	return CurrentLifeAmount <= 0.0f;
}

float AShooterCharacter::GetHealthPercent() const
{
	return CurrentLifeAmount / MaxLifeAmount;
}

void AShooterCharacter::MoveForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector(), axisValue);
}

void AShooterCharacter::MoveRight(float axisValue)
{
	AddMovementInput(GetActorRightVector(), axisValue);
}

void AShooterCharacter::GamepadLookUp(float axisValue)
{
	AddControllerPitchInput(axisValue * RotationRate * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void AShooterCharacter::GamepadLookRight(float axisValue)
{
	AddControllerYawInput(axisValue * RotationRate * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void AShooterCharacter::PullTrigger()
{
	if (Gun)
		Gun->PullTrigger();
}

void AShooterCharacter::ReleaseTrigger()
{
	if (Gun)
		Gun->ReleaseTrigger();
}

