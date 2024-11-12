// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API AFlag : public AWeapon
{
	GENERATED_BODY()
public:
	AFlag();
	virtual void Dropped() override;
	void ResetFlag();
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FlagMesh;
	FTransform InitialTransform;
protected:
	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	virtual void BeginPlay() override;
public:
	FORCEINLINE FTransform GetIntialTransform() const {return  InitialTransform; }
};
