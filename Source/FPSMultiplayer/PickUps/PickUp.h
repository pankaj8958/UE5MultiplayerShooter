// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UCLASS()
class FPSMULTIPLAYER_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUp();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;
private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;
	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;
};
