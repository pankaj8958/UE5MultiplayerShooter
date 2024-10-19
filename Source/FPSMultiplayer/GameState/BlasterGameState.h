// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPSMultiplayer/PlayerState/BlasterPlayerState.h"
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
	TArray<ABlasterPlayerState*> TopScoringPlayers;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ABlasterPlayerState* ScorePLayer);

	void RedTeamScores();
	void BlueTeamScores();
	TArray<ABlasterPlayerState*> RedTeam;
	TArray<ABlasterPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();
	
	UFUNCTION()
	void OnRep_BlueTeamScore();
private:
	float TopScore = 0.f;
};
