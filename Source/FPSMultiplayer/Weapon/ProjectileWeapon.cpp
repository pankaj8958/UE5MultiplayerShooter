// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include  "Projectile.h"
void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	UWorld* World = GetWorld();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotate = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		

		AProjectile* SpawnProjectile = nullptr;
		if(bUserServerSideRewind)
		{
			if(InstigatorPawn->HasAuthority())
			{
				if(InstigatorPawn->IsLocallyControlled())
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotate, SpawnParams);
					SpawnProjectile->bUseServerSideRewind = false;
					SpawnProjectile->Damage = Damage;
				}
				else
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotate, SpawnParams);
					SpawnProjectile->bUseServerSideRewind = false;
				}
			}
			else
			{
				if(InstigatorPawn->IsLocallyControlled())
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotate, SpawnParams);
					SpawnProjectile->bUseServerSideRewind = false;
					SpawnProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnProjectile->InitialVelocity = SpawnProjectile->GetActorForwardVector() * SpawnProjectile->InitialSpeed;
					SpawnProjectile->Damage = Damage;
				}
				else
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotate, SpawnParams);
					SpawnProjectile->bUseServerSideRewind = true;
				}
			}
		}
		else
		{
			if(InstigatorPawn->HasAuthority())
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotate, SpawnParams);
				SpawnProjectile->bUseServerSideRewind = false;
				SpawnProjectile->Damage = Damage;
			}
		}
	}
}


