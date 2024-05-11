// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
    IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
    if(subsystem)
    {
        sessionInterface = subsystem->GetSessionInterface();
    }
}

void UMultiplayerSessionSubsystem::CreateSession(int32 numPublicConnection, FString matchType)
{
    if(!sessionInterface.IsValid())
    {
        return;
    }
    auto existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
    if(existingSession)
    {
        bCreateSessionOnDestroy = true;
        lastNumPublicConnections = numPublicConnection;
        lastMatchType = matchType;
        DestroySession();
    }
    createSessionCompleteDelegate = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
    
    lastSessionSettings = MakeShareable(new FOnlineSessionSettings());

    lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSettings->NumPublicConnections = numPublicConnection;
	lastSessionSettings->bAllowJoinInProgress = true;
	lastSessionSettings->bAllowJoinViaPresence = true;
	lastSessionSettings->bShouldAdvertise = true;
	lastSessionSettings->bUsesPresence = true;
	lastSessionSettings->bUseLobbiesIfAvailable = true;
    lastSessionSettings->BuildUniqueId = 1;
    
	lastSessionSettings->Set(FName("matchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if(!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *lastSessionSettings))
    {
        sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);
        multiplayerOnCreateSessionComplete.Broadcast(false);
    }
}

void UMultiplayerSessionSubsystem::FindSessions(int32 maxSearchResult)
{
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("Find Session 1"))
        );
    }
    if(!sessionInterface.IsValid())
    {
        return;
    }
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("Find Session 2"))
        );
    }
    findSessionCompleteDelegate = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);
    lastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    lastSessionSearch->MaxSearchResults = maxSearchResult;
    lastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
    lastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if(!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), lastSessionSearch.ToSharedRef()))
    {
        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Yellow,
                FString(TEXT("Find Session 3"))
            );
        }
        sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionCompleteDelegate);
        multiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
    }
}

void UMultiplayerSessionSubsystem::JoinSession(const FOnlineSessionSearchResult &sessionResult)
{
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("JoinSession Session 3"))
        );
    }
    if(!sessionInterface.IsValid())
    {
        multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
        return;
    }
    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    joinSessionCompleteDelegate = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
    if(!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionResult))
    {
        sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);
        multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
    }
}

void UMultiplayerSessionSubsystem::DestroySession()
{
    if(!sessionInterface.IsValid())
    {
        multiplayerOnDestroySessionComplete.Broadcast(false);
        return;
    }

    destroySessionCompleteDelegate = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

    if(!sessionInterface->DestroySession(NAME_GameSession))
    {
        sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);
        multiplayerOnDestroySessionComplete.Broadcast(false);
    }
}

void UMultiplayerSessionSubsystem::StartSession()
{
    if(!sessionInterface.IsValid())
    {
        return;
    }
    startSessionCompleteDelegate = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

    if(!sessionInterface->StartSession(NAME_GameSession))
    {
        sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(startSessionCompleteDelegate);
        multiplayerOnStartSessionComplete.Broadcast(false);
    }
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
    if(sessionInterface)
    {
        sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);
    }
    multiplayerOnCreateSessionComplete.Broadcast(true);
}

void UMultiplayerSessionSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
    if(sessionInterface)
    {
        sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionCompleteDelegate);
    }
    if(lastSessionSearch->SearchResults.Num() <= 0)
    {
        multiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }
    multiplayerOnFindSessionComplete.Broadcast(lastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
    if(sessionInterface.IsValid())
    {
        sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);
    }
    multiplayerOnJoinSessionComplete.Broadcast(result);
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
    if(sessionInterface)
    {
        sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);
    }
    if(bWasSuccessful && bCreateSessionOnDestroy)
    {
        bCreateSessionOnDestroy = false;
        CreateSession(lastNumPublicConnections, lastMatchType);
    }
    multiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::OnStartSessionComplete(FName sessionName, bool bWasSuccessful)
{
    if(sessionInterface)
    {
        sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(startSessionCompleteDelegate);
    }
    if(bWasSuccessful)
    {
        CreateSession(lastNumPublicConnections, lastMatchType);
    }
    multiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
}
