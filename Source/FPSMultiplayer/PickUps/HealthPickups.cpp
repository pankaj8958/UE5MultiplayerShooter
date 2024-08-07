// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickups.h"
#include "FPSMultiplayer/Components/BuffComponent.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"

AHealthPickups::AHealthPickups()
{
	bReplicates = true;
}
void AHealthPickups::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, otherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(otherActor);
	if(BlasterCharacter)
	{
		
	}
	Destroy();
}

