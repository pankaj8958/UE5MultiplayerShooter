// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FPSMultiplayer/Widget/BlasterHUD.h"
#include "FPSMultiplayer/Widget/CharacterOverlay.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}
void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
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
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmt)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmt->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Value)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmt)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Value);
		BlasterHUD->CharacterOverlay->DefeatsAmt->SetText(FText::FromString(DefeatsText));
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
void ABlasterPlayerController::SetHUDTime()
{
	uint32 SecLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if(CountdownInt != SecLeft)
	{
		SetMatchCountDown(MatchTime - GetServerTime());
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


