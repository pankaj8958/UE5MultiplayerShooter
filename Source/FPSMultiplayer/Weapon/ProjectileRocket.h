// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class FPSMULTIPLAYER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileRocket();
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
private:
	UPROPERTY(EditAnywhere, Category=Projectile)
	float MinDamage = 2.f;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
};
