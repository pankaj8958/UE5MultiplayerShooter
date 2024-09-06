// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include  "FPSMultiplayer/Components/LagCompensationComponent.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn)
		return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;

		FHitResult FireHit;
		UWorld* World = GetWorld();
		if(World)
		{
			World->LineTraceSingleByChannel(
				FireHit,
				Start,
				End,
				ECC_Visibility
				);
			if(FireHit.bBlockingHit)
			{
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
				if(BlasterCharacter)
				{
					if(HasAuthority() && !bUserServerSideRewind)
					{
						UGameplayStatics::ApplyDamage(BlasterCharacter,Damage,
						InstigatorController,this, UDamageType::StaticClass());
					}
					if(!HasAuthority() && bUserServerSideRewind)
					{
						BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
						BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(OwnerPawn) : BlasterOwnerController;
						if(BlasterOwnerCharacter && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation())
						{
							BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
								BlasterCharacter,
								Start,
								HitTarget,
								BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
								this
							);
						}
					}
					if(ImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(
							World,
							ImpactParticles,
							FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation());
					}
				}
			}
		}
	}
}
