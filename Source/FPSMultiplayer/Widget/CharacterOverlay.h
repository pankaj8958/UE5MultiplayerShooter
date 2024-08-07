// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthText;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ShieldBar;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ShieldText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ScoreAmt;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DefeatsAmt;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeaponAmmoAmt;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CarryAmmoAmt;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MatchCountdownText;
	
};
