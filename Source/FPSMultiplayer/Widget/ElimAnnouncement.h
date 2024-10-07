// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "ElimAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetElimAnnouncementText(FString AttackerName, FString VictimName);
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AnnouncementBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;

	
};
