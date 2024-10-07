// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/GameState/BlasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "FPSMultiplayer/PlayerState/BlasterPlayerState.h"
namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}
ABlasterGameMode::ABlasterGameMode()
{
    bDelayedStart = true;
}
void ABlasterGameMode::BeginPlay()
{
    Super::BeginPlay();
    LevelStartingTime = GetWorld()->GetTimeSeconds();
}
void ABlasterGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(MatchState == MatchState::WaitingToStart)
    {
        CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if(CountDownTime <= 0.f)
        {
            StartMatch();
        }
    }
    else if(MatchState == MatchState::InProgress)
    {
        CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if(CountDownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if(MatchState == MatchState::Cooldown)
    {
        CooldownTime = CooldownTime + WarmupTime+MatchTime-GetWorld()->GetTimeSeconds()+LevelStartingTime;
        if(CountDownTime <= 0.f)
        {
            RestartGame();
        }
    }
}
void ABlasterGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();
    for( FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if(BlasterPlayer)
        {
            BlasterPlayer->OnMatchStateSet(MatchState);
        }
    }
}
void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
    if(AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
    if(VictimController == nullptr || VictimController->PlayerState == nullptr) return;

    ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
    if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
    {
        TArray<ABlasterPlayerState*> PlayersCurrentlyInLead;
        for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
        {
            PlayersCurrentlyInLead.Add(LeadPlayer);
        }
        
        AttackerPlayerState->AddToScore(1.f);
        BlasterGameState->UpdateTopScore(AttackerPlayerState);

        if(BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
        {
            ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
            if(Leader)
            {
                Leader->MulticastGainedTheLead();
            }
        }
        for (int32 i = 0; i < PlayersCurrentlyInLead.Num(); i++)
        {
            if(BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInLead[i]))
            {
                ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInLead[i]->GetPawn());
                if(Loser)
                {
                    Loser->MulticastGainedTheLead();
                }
            }
        }
    }
    if(VictimPlayerState)
    {
        VictimPlayerState->AddDefeats(1);
    }
    if(ElimedCharacter)
    {
        ElimedCharacter->Eliminate(false);
    }
    for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; ++IT)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*IT);
        if(BlasterPlayer && AttackerPlayerState && VictimPlayerState)
        {
            BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
        }
    }
}

void ABlasterGameMode::RequestRespawn(ACharacter *ElimCharacter, AController *ElimController)
{
    if(ElimCharacter)
    {
        ElimCharacter->Reset();
        ElimCharacter->Destroy();
    }
    if(ElimController)
    {
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimController, PlayerStarts[Selection]);
    }
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
    if(PlayerLeaving == nullptr) return;
    ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
    if(BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
    {
        BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
    }
    ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
    if(CharacterLeaving)
    {
        CharacterLeaving->Eliminate(true);
    }
}

