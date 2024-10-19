// Fill out your copyright notice in the Description page of Project Settings.
#include "BlasterPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FPSMultiplayer/BlasterType/Announcement.h"
#include "FPSMultiplayer/Widget/BlasterHUD.h"
#include "FPSMultiplayer/Widget/CharacterOverlay.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/GameMode/BlasterGameMode.h"
#include "FPSMultiplayer/Widget/Announcement.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FPSMultiplayer/Components/CombatCompoment.h"
#include "FPSMultiplayer/GameState/BlasterGameState.h"
#include "FPSMultiplayer/PlayerState/BlasterPlayerState.h"
#include "FPSMultiplayer/Widget/ReturnToMainMenu.h"

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
	CheckPing(DeltaSeconds);
}

void ABlasterPlayerController::HideTeamScore()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->RedTeamScore && BlasterHUD->CharacterOverlay->BlueTeamScore&& BlasterHUD->CharacterOverlay->ScoreSpacerText)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());
	}
}

void ABlasterPlayerController::InitTeamScore()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->RedTeamScore && BlasterHUD->CharacterOverlay->BlueTeamScore&& BlasterHUD->CharacterOverlay->ScoreSpacerText)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString("0"));
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString("0"));
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString("|"));
	}
}

void ABlasterPlayerController::SetHudRedTeamScore(int32 RedTeamScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->RedTeamScore)
	{
		FString ScoreText = FString::Printf(TEXT("%d"),RedTeamScore); 
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHudBlueTeamScore(int32 BlueTeamScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->BlueTeamScore)
	{
		FString ScoreText = FString::Printf(TEXT("%d"),BlueTeamScore); 
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
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
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ShieldBar && BlasterHUD->CharacterOverlay->ShieldText)
	{
		const float ShieldPercent = Shield/MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldString));
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
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
		bInitializeScore = true;
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
		bInitializeDefeat = true;
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
		SetHUDShield(BlasterCharacter->GetShild(), BlasterCharacter->GetMaxShield());
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
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Value;
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
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Value;
	}
}
void ABlasterPlayerController::SetMatchCountDown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountdownText)
	{
		if(CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
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
		if(CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
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
	else if(MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartTime;
	uint32 SecLeft = FMath::CeilToInt(TimeLeft);
	if(HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if(BlasterGameMode)
		{
			SecLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartTime);
		}
	}
	if(CountdownInt != SecLeft)
	{
		if(MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(InputComponent == nullptr) return;
	InputComponent->BindAction("Quit", IE_Pressed, this, &ABlasterPlayerController::ShowReturnToMenu);
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
				if(bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if(bInitializeShield)SetHUDShield(HUDShield, HUDMaxShield);
				if(bInitializeScore)SetHUDScore(HUDScore);
				if(bInitializeDefeat)SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarryAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
			}
		}
	}
}
void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	if(HasAuthority()) return;
	HighPingRunningTime += DeltaTime;
	if(HighPingRunningTime > CheckPingFrequency)
	{
		if(GetPlayerState<APlayerState>())
		{
			if(GetPlayerState<APlayerState>()->GetPingInMilliseconds() > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			} else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	if(BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
	{
		PingAnimationRunningTime += DeltaTime;
		if(PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}
void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HighPingImage
		&& BlasterHUD->CharacterOverlay->HighPingAnimation)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation,
			0.f, 5);
	}
}
void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HighPingImage
		&& BlasterHUD->CharacterOverlay->HighPingAnimation)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if(BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
	}
}

void ABlasterPlayerController::ShowReturnToMenu()
{
	if(ReturnMainmenuWidget == nullptr) return;
	if(ReturnToMenu == nullptr)
	{
		ReturnToMenu = CreateWidget<UReturnToMainMenu>(this, ReturnMainmenuWidget);
	}
	if(ReturnToMenu)
	{
		bReturnToMainMenu = !bReturnToMainMenu;
		if(bReturnToMainMenu)
		{
			ReturnToMenu->MenuSetup();
		}
		else
		{
			ReturnToMenu->MenuTearDown();
		}
	}
}

void ABlasterPlayerController::OnRep_ShowTeamScore()
{
	if(bShowteamScore)
	{
		InitTeamScore();	
	}
	else
	{
		HideTeamScore();
	}
}

FString ABlasterPlayerController::GetInfotext(const TArray<ABlasterPlayerState*>& Players)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if(BlasterPlayerState == nullptr) return FString();
	FString InfoTextString;
	if(Players.Num() == 0)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if(Players.Num() == 1 && Players[0] == BlasterPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if(Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if(Players.Num() > 1)
	{
		InfoTextString = Announcement::PlayerTiesForWin;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"),*TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;
}

FString ABlasterPlayerController::GetTeamInfotext(ABlasterGameState* BlasterGameSTate)
{
	if(BlasterGameSTate == nullptr) return FString();
	FString InfoTextString;

	const int32 RedTeamScore = BlasterGameSTate->RedTeamScore;
	const int32 BlueTeamScore = BlasterGameSTate->BlueTeamScore;
	if(RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if(RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamTiesForWin);
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append("\n");
	}
	else if(RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString = FString::Printf(TEXT("%s : %d \n"), *Announcement::RedTeam, RedTeamScore);
		InfoTextString = FString::Printf(TEXT("%s : %d \n"), *Announcement::BlueTeam, BlueTeamScore);
	}
	else if(BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Announcement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString = FString::Printf(TEXT("%s : %d \n"), *Announcement::BlueTeam, BlueTeamScore);
		InfoTextString = FString::Printf(TEXT("%s : %d \n"), *Announcement::RedTeam, RedTeamScore);
	}
	return  InfoTextString;
}

void ABlasterPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}
void ABlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if(Attacker && Victim && Self)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if(BlasterHUD)
		{
			if(Attacker == Self && Victim != Self)
			{
				BlasterHUD->ElimAnnouncent("You", Victim->GetPlayerName());
				return;;
			}
			if(Victim == Self && Attacker != Self)
			{
				BlasterHUD->ElimAnnouncent( Attacker->GetPlayerName(), "You");
				return;;
			}
			if(Victim == Attacker && Attacker == Self)
			{
				BlasterHUD->ElimAnnouncent( "You", "yourself");
				return;;
			}
			if(Victim == Attacker && Attacker != Self)
			{
				BlasterHUD->ElimAnnouncent( Attacker->GetPlayerName(), "Themselve");
				return;;
			}
			BlasterHUD->ElimAnnouncent( Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}
void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateofMatch, float Warmup, float Match,
                                                                float StatingTime, float Cooltime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartTime = StatingTime;
	MatchState = StateofMatch;
	CooldownTime = Cooltime;
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
		CooldownTime = GameMode->CooldownTime;
		MatchTime = GameMode->MatchTime;
		LevelStartTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartTime, CooldownTime);
	}
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientReq,
                                                                     float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->TimeSeconds - TimeOfClientReq;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurretServerTime = TimeServerReceivedRequest + SingleTripTime;
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
void ABlasterPlayerController::OnMatchStateSet(FName State, bool bteamMatch)
{
	MatchState = State;
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bteamMatch);
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}
void ABlasterPlayerController::OnRep_MatchState()
{
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABlasterPlayerController::HandleMatchHasStarted(bool bteamMatch)
{
	if(HasAuthority()) bShowteamScore = bteamMatch;
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD)
	{
		if(BlasterHUD->CharacterOverlay == nullptr)
			BlasterHUD->AddCharacterOverlay();
		if(BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if(!HasAuthority()) return;
		if(bteamMatch)
		{
			InitTeamScore();
		}
		else
		{
			HideTeamScore();
		}
	}
}
void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		if(BlasterHUD->Announcement
			&& BlasterHUD->Announcement->AnnouncementText
			&& BlasterHUD->Announcement->InfoText)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementString = Announcement::NewMatchStartsIn;
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementString));
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if(BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString = bShowteamScore ? GetTeamInfotext(BlasterGameState) : GetInfotext(TopPlayers);
				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisplayGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}




