// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamBasedGameMode.h"

#include "FPSMultiplayer/GameState/BlasterGameState.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATeamBasedGameMode::ATeamBasedGameMode()
{
	bTeamsMatch = true;
}

void ATeamBasedGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if(BGameState)
	{
		ABlasterPlayerState* BPState = NewPlayer->GetPlayerState<ABlasterPlayerState>();
		if(BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if(BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamBasedGameMode::Logout(AController* Exiting)
{
	ABlasterGameState* BGamestate = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Exiting->GetPlayerState<ABlasterPlayerState>();
	if(BGamestate && BPState)
	{
		if(BGamestate->RedTeam.Contains(BPState))
		{
			BGamestate->RedTeam.Remove(BPState);
		}
		if(BGamestate->BlueTeam.Contains(BPState))
		{
			BGamestate->BlueTeam.Remove(BPState);
		}
	}
}

float ATeamBasedGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ABlasterPlayerState* AttackerPState = Attacker->GetPlayerState<ABlasterPlayerState>();
	ABlasterPlayerState* VictimPState = Victim->GetPlayerState<ABlasterPlayerState>();
	if(AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if(VictimPState == AttackerPState)
	{
		return  BaseDamage;
	}
	if(AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}

void ATeamBasedGameMode::PlayerEliminated(ABlasterCharacter* ElimedCharacter,
	ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimedCharacter, VictimController, AttackerController);
	ABlasterGameState* BGamestate = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* AttackeGameState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	if(BGamestate && AttackeGameState)
	{
		if(AttackeGameState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGamestate->BlueTeamScores();
		}
		if(AttackeGameState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGamestate->RedTeamScores();
		}
	}
}

void ATeamBasedGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	ABlasterGameState* BGamestate = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if(BGamestate)
	{
		for (auto PState : BGamestate->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			if(BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if(BGamestate->BlueTeam.Num() >= BGamestate->RedTeam.Num())
				{
					BGamestate->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGamestate->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
