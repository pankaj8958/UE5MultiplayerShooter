// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureFlagGameMode.h"

#include "FPSMultiplayer/CaptureFlag/FlagZone.h"
#include "FPSMultiplayer/GameState/BlasterGameState.h"
#include "FPSMultiplayer/Weapon/Flag.h"

void ACaptureFlagGameMode::PlayerEliminated(ABlasterCharacter* ElimedCharacter,
                                            ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimedCharacter, VictimController, AttackerController);
}

void ACaptureFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(GameState);
	if(BGameState)
	{
		if(Zone->Team == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		if(Zone->Team == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
}
