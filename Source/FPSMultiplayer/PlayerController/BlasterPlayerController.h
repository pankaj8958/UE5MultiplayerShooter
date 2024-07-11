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
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Value);
	void SetHUDWeaponAmmo(int32 Value);
	void SetHUDCarryAmmo(int32 Value);
	void SetMatchCountDown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
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
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
};
