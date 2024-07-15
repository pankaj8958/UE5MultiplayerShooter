// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FPSMultiplayer/Widget/BlasterHUD.h"
#include "FPSMultiplayer/Widget/CharacterOverlay.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/GameMode/BlasterGameMode.h"
#include "FPSMultiplayer/Widget/Announcement.h"
#include  "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
}
void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}
void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health/MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthString));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmt)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmt->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Value)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmt)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Value);
		BlasterHUD->CharacterOverlay->DefeatsAmt->SetText(FText::FromString(DefeatsText));
	}else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Value;
	}
}
void ABlasterPlayerController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if(BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Value)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->WeaponAmmoAmt)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Value);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmt->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarryAmmo(int32 Value)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->CarryAmmoAmt)
	{
		FString CarryText = FString::Printf(TEXT("%d"), Value);
		BlasterHUD->CharacterOverlay->CarryAmmoAmt->SetText(FText::FromString(CarryText));
	}
}
void ABlasterPlayerController::SetMatchCountDown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountdownText)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncement(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->Announcement && BlasterHUD->Announcement->WarmupTime)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if(MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime-GetServerTime() + LevelStartTime;
	else if(MatchState == MatchState :: InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartTime;
	
	uint32 SecLeft = FMath::CeilToInt(TimeLeft);
	if(CountdownInt != SecLeft)
	{
		if(MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncement(TimeLeft);
		}
		if(MatchState == MatchState::InProgress)
		{
			SetMatchCountDown(TimeLeft);
		}
	}
	CountdownInt = SecLeft;
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSynchFreq += DeltaTime;
	if(IsLocalController() && TimeSyncRunningTime > TimeSynchFreq)
	{
			ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}
void ABlasterPlayerController::PollInit()
{
	if(CharacterOverlay == nullptr)
	{
		if(BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if(CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateofMatch, float Warmup, float Match,
	float StatingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartTime = StatingTime;
	MatchState = StateofMatch;
	OnMatchStateSet(MatchState);
	if(BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartTime);
	}
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientReq,
                                                                     float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->TimeSeconds - TimeOfClientReq;
	float CurretServerTime = TimeServerReceivedRequest + (0.5f*RoundTripTime);
	ClientServerDelta = CurretServerTime - GetWorld()->GetTimeSeconds();
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeReceipt = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	ClientReportServerTime(TimeOfClientRequest, ServerTimeReceipt);
}
float ABlasterPlayerController::GetServerTime()
{
	if(HasAuthority()) return  GetWorld()->GetTimeSeconds();
	else return  GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}
void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();
		if(BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}



