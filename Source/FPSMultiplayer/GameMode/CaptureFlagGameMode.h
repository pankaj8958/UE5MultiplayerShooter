// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamBasedGameMode.h"
#include "CaptureFlagGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API ACaptureFlagGameMode : public ATeamBasedGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(ABlasterCharacter* ElimedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};
