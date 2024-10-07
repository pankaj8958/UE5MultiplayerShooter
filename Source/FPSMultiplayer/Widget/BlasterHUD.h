// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElimAnnouncement.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCentre;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};
UCLASS()
class FPSMULTIPLAYER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual  void DrawHUD() override;
	UPROPERTY(EditAnywhere, Category="Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();
	UPROPERTY(EditAnywhere, Category=Announcement)
	TSubclassOf<UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();
	void ElimAnnouncent(FString Attacker, FString Victim);
	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTimer = 2.5f;
	UFUNCTION()
	void ElimAnnouncementTimerFinish(UElimAnnouncement* MsgToRemove);
	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;
protected:
	virtual void BeginPlay() override;
	
private:
	class APlayerController* OwningPlayer;
	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre, FVector2D Spread, FLinearColor CrosshairColor);
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) {HUDPackage = Package;}
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
};
