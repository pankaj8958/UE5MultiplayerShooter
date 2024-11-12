// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"

#include "Components/SphereComponent.h"
#include "FPSMultiplayer/GameMode/CaptureFlagGameMode.h"
#include "FPSMultiplayer/Weapon/Flag.h"

// Sets default values
AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);
}

// Called when the game starts or when spawned
void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlapCompo, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);
	if(OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		ACaptureFlagGameMode* CaptureMode = GetWorld()->GetAuthGameMode<ACaptureFlagGameMode>();
		if(CaptureMode)
		{
			CaptureMode->FlagCaptured(OverlappingFlag, this);
		}
		OverlappingFlag->ResetFlag();
	}
}

