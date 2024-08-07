// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
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
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();

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

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateofMatch, float Warmup, float Match, float StatingTime, float Cooltime);
	
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
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
	
};
