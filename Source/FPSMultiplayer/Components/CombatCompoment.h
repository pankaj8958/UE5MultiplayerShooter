// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSMultiplayer/Widget/BlasterHUD.h"
#include "FPSMultiplayer/Weapon/WeaponTypes.h"
#include "Containers/Map.h"
#include "FPSMultiplayer/BlasterType/CombatState.h"
#include "CombatCompoment.generated.h"
#define  TRACE_LENGTH = 80000.f
class AWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSMULTIPLAYER_API UCombatCompoment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatCompoment();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	friend  class ABlasterCharacter;
	void EquipWeapon(AWeapon* WeaponToEquip);
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool IsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void FireButtonPressed(bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrossHairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();
	int32 AmountToReload();
private:
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* PlayerController;
	class ABlasterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;
	UFUNCTION()
	void OnRep_Aiming();
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	FVector HitTarget;
	FHUDPackage HUDPackage;
	//FOV / Zoom
	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category=Combat)
	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere, Category=Combat)
	float ZoomInterpSpeed = 20.f;
	float CurrentFOV = 20.f;
	void InterpFOV(float Deltatime);
	FTimerHandle FireTimer;
	bool bCanFire = true;
	void FireTimerStart();
	void FireTimerFinished();
	void Fire();

	bool CanFire();
	UPROPERTY(ReplicatedUsing = OnRep_CarryAmmo)
	int32 CarryAmmo;
	UFUNCTION()
	void OnRep_CarryAmmo();
	TMap<EWeaponType, int32> CarryAmmoMap;

	UPROPERTY(EditAnywhere)	
	int32 StartingAmmo = 30;
	
	void InitializeCarryAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatType CombatState = ECombatType::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
};
