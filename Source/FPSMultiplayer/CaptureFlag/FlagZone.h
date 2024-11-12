// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSMultiplayer/GameMode/Team.h"
#include "GameFramework/Actor.h"
#include "FlagZone.generated.h"

UCLASS()
class FPSMULTIPLAYER_API AFlagZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlagZone();
	UPROPERTY(EditAnywhere)
	ETeam Team;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlapCompo,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY(EditAnywhere)
	class USphereComponent* ZoneSphere;
};
