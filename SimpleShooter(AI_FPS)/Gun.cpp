// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/HitResult.h"
#include "Engine/DamageEvents.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
}

void AGun::PullTrigger()
{
	CurrentBulletsNumber = BulletPerShot;
	Shoot();

	if (!bIsAutoOrMultipleRound)
		return;

	GetWorldTimerManager().SetTimer(TimerHandler, [this]()
		{
			Shoot();

			if (BulletPerShot > 0) ResetShoot();

		}, FireRate, true);
}

void AGun::ReleaseTrigger()
{
	if (bIsAutoOrMultipleRound && BulletPerShot == 0)
		GetWorldTimerManager().ClearTimer(TimerHandler);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::Shoot()
{
	CurrentBulletsNumber--;

	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, MeshComponent, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComponent, TEXT("MuzzleFlashSocket"));
	
	FHitResult hit;
	FVector shotDirection;
	if (!BulletTrace(hit, shotDirection)) return;

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, hit.Location, shotDirection.Rotation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, hit.Location);

	AActor* actor = hit.GetActor();
	if (!actor)
		return;

	AController* OwnerController = GetOwnerController();

	if (!OwnerController) return;

	FPointDamageEvent damageEvent(DamagePerBullet, hit, shotDirection, nullptr);
	actor->TakeDamage(DamagePerBullet, damageEvent, OwnerController, this);
}

void AGun::ResetShoot()
{
	if (CurrentBulletsNumber == 0)
		GetWorldTimerManager().ClearTimer(TimerHandler);
}

AController* AGun::GetOwnerController() const
{
	APawn* owner = Cast<APawn>(GetOwner());
	if (!owner) return nullptr;

	return owner->GetController();
}

bool AGun::BulletTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return false;

	FVector Location;
	FRotator Rotation;

	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector end = Location + Rotation.Vector() * Range;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(HitResult, Location, end, ECC_GameTraceChannel1, Params);
}

