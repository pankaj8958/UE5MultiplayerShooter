// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "FPSMultiplayer/PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
    if(AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
    if(VictimController == nullptr || VictimController->PlayerState == nullptr) return;

    ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	
    if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
    {
        AttackerPlayerState->AddToScore(1.f);
    }
    if(VictimPlayerState)
    {
        VictimPlayerState->AddDefeats(1);
    }
    if(ElimedCharacter)
    {
        ElimedCharacter->Eliminate();
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
