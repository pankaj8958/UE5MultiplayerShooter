// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(Blueprintable)
struct FBoxInfo
{
	GENERATED_BODY()
	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};
USTRUCT()
struct FFramePackage
{
	GENERATED_BODY()
	UPROPERTY()
	float time;
	UPROPERTY()
	TMap<FName, FBoxInfo> HitBoxInfo;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSMULTIPLAYER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	
	UPROPERTY()
	class ABlasterPlayerController* Controller;
};
