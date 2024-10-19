// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamBasedGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API ATeamBasedGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
public:
	ATeamBasedGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(ABlasterCharacter* ElimedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;
protected:
	virtual void HandleMatchHasStarted() override;
};