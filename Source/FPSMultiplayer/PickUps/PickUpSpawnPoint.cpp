// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpSpawnPoint.h"
#include "PickUp.h"

APickUpSpawnPoint::APickUpSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void APickUpSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnPickupTimer((AActor*)nullptr);
}

void APickUpSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void APickUpSpawnPoint::SpawnPickup()
{
	int32 NumPickupClasses = PickUpClasses.Num();
	if(NumPickupClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumPickupClasses - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickUp>(PickUpClasses[Selection], GetActorTransform());
		if(HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickUpSpawnPoint::StartSpawnPickupTimer);
		}
	}
}
void APickUpSpawnPoint::SpawnPickupTimerFinished()
{
	if(HasAuthority())
	{
		SpawnPickup();
	}
}
void APickUpSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickupTimerMin, SpawnPickupTimerMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&APickUpSpawnPoint::SpawnPickupTimerFinished,
		SpawnTime
	);
}


