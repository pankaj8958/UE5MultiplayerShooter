// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
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
