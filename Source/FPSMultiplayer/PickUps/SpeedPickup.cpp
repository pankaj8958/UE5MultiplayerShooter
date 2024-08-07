// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickup.h"

#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/Components/BuffComponent.h"

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, otherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(otherActor);
	if(BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if(Buff)
		{
			Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
		}
	}
	Destroy();
}
