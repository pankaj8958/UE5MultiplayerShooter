// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABlasterPlayerState, Defeats);
}
void ABlasterPlayerState::OnRep_Defeats()
{

}
void ABlasterPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if(Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if(Controller)
        {

        }
    }
}
void ABlasterPlayerState::AddDefeats(int32 Value)
{
    Defeats += Value;
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if(Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if(Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}
void ABlasterPlayerState::AddToScore(float ScoreAmt)
{
    SetScore(GetScore() + ScoreAmt);
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if(Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if(Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}
