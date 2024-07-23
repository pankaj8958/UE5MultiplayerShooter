// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated)
	TArray<class ABlasterPlayerState*> TopScoringPlayers;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ABlasterPlayerState* ScorePLayer);
private:
	float TopScore = 0.f;
};
