// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "HealthPickups.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API AHealthPickups : public APickUp
{
	GENERATED_BODY()
public:
	AHealthPickups();
protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere)
	float HealAmmount = 100.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;
	
};
