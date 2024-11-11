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
	void FireButtonPressed(bool bPressed);
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	bool ShouldSwapWeapons();
	bool bLocallyReloading = false;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool IsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrossHairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();
	int32 AmountToReload();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void AttachFlagToLeftHand(AWeapon* Flag);
private:
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* PlayerController;
	ABlasterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;
	UFUNCTION()
	void OnRep_Aiming();
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	bool bAimButtonPressed = false;
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
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	bool CanFire();
	UPROPERTY(ReplicatedUsing = OnRep_CarryAmmo)
	int32 CarryAmmo;
	UFUNCTION()
	void OnRep_CarryAmmo();
	TMap<EWeaponType, int32> CarryAmmoMap;
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	
	UPROPERTY(EditAnywhere)	
	int32 StartingAmmo = 30;
	
	UPROPERTY(EditAnywhere)	
	int32 StartingRocketAmmo = 30;

	UPROPERTY(EditAnywhere)	
	int32 StartingPistolAmmo = 30;
	void InitializeCarryAmmo();
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatType CombatState = ECombatType::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();
	void UpdateAmmoValues();
	void SwapWeapons();
	UPROPERTY(ReplicatedUsing="OnRep_HoldingTheFlag")
	bool bHoldingTheFlag = false;
	UFUNCTION()
	void OnRep_HoldingTheFlag();
	UPROPERTY()
	AWeapon* TheFlag;
};
