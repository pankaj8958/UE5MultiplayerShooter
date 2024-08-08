// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpSpawnPoint.generated.h"

UCLASS()
class FPSMULTIPLAYER_API APickUpSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:
	APickUpSpawnPoint();
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickUp>> PickUpClasses;
	UPROPERTY()
	APickUp* SpawnedPickup;

	void SpawnPickup();
	void SpawnPickupTimerFinished();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

private:
	FTimerHandle SpawnPickupTimer;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimerMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimerMax;

};
