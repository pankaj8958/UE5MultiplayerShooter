// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ABlasterCharacter* ElimedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimCharacter, AController* ElimController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	float LevelStartingTime;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountDownTime = 0.f;
};
