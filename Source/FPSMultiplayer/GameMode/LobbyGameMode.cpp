// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "MultiplayerSessionSubsystem.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);
    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
    UGameInstance* GameInstance = GetGameInstance();
    if(GameInstance)
    {
        UMultiplayerSessionSubsystem* SubSystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
        check(SubSystem);

        if(NumberOfPlayers == SubSystem->DesiredNumPublicConnections)
        {
            UWorld* World = GetWorld();
            if(World)
            {
                //TODO all different scene for different mode
                bUseSeamlessTravel = true;
                FString Matchtype = SubSystem->DesiredMatchType;
                if(Matchtype == "FreeForAll")
                {
                    World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
                }
                else if(Matchtype == "Teams")
                {
                    World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
                }
                else if(Matchtype == "CaptureTheFlag")
                {
                    World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
                }
            }
        }
    }

}
