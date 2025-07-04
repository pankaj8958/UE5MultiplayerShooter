// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSMultiplayer/PlayerState/BlasterPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

UCLASS()
class FPSMULTIPLAYER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Value);
	void SetHUDWeaponAmmo(int32 Value);
	void SetHUDCarryAmmo(int32 Value);
	void SetMatchCountDown(float CountdownTime);
	void SetHUDAnnouncement(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	void HideTeamScore();
	void InitTeamScore();
	void SetHudRedTeamScore(int32 RedTeamScore);
	void SetHudBlueTeamScore(int32 BlueTeamScore);
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State, bool bteamMatch = false);
	void HandleMatchHasStarted(bool bteamMatch = false);
	void HandleCooldown();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float SingleTripTime = 0.f;
	FHighPingDelegate HighPingDelegate;
	
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	virtual void SetupInputComponent() override;
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientReq, float TimeServerReceivedRequest);
	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSynchFreq = 5.f;
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);
	void PollInit();
	void CheckPing(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateofMatch, float Warmup, float Match, float StatingTime, float Cooltime);
	void HighPingWarning();
	void StopHighPingWarning();
	void ShowReturnToMenu();
	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);
	UPROPERTY(ReplicatedUsing=OnRep_ShowTeamScore)
	bool bShowteamScore = false;
	UFUNCTION()
	void OnRep_ShowTeamScore();
	FString GetInfotext(const TArray<ABlasterPlayerState*>& Players);
	FString GetTeamInfotext(class ABlasterGameState* BlasterGameSTate);
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<class UUserWidget> ReturnMainmenuWidget;
	UPROPERTY()
	class UReturnToMainMenu* ReturnToMenu;
	bool bReturnToMainMenu = false;
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	float MatchTime = 0.f;
	float LevelStartTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializeHealth = false;
	float HUDScore;
	bool bInitializeScore = false;
	float HUDShield;
	bool bInitializeShield = false;
	float HUDMaxShield;
	
	int32 HUDDefeats;
	bool bInitializeDefeat = false;
	
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;
	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
	float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;
	float PingAnimationRunningTime = 0.f;
};
