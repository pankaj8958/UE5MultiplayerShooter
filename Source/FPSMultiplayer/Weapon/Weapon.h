// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"
UENUM(BLueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "InitialState"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Max UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class FPSMULTIPLAYER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void OnRep_Owner() override;
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& HitTarget);
	UPROPERTY(EditAnywhere, Category=Combat)
	float FireDelay = 0.15f;
	UPROPERTY(EditAnywhere, Category=Combat)
	bool bAutomatic = true;
	void Dropped();
	void SetHudWeaponAmmo();
	void AddAmmo(int32 AmmoAmt);
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
	void EnableCustomDepth(bool bEnabled);
	bool bDestroyWeapon = false;
protected:
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere, Replicated)
	bool bUserServerSideRewind = false;
	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState ,VisibleAnywhere,  Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();
	
	UPROPERTY(VisibleAnywhere,  Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, Category="Weapon property")
	class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
	int32 Sequence = 0;//Number of unprocess server request send for ammo
	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoAmt);
	
	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	//FOV / Zoom
	UPROPERTY(EditAnywhere)
	float ZoomFov = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	class UTexture2D* CrossHairsCentre;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	class UTexture2D* CrossHairsLeft;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	class UTexture2D* CrossHairsRight;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	class UTexture2D* CrossHairsTop;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	class UTexture2D* CrossHairsBottom;
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomFov;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	bool IsEmpty();
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE int32 GetAmmo() const {return  Ammo;}
	FORCEINLINE int32 GetMagCapacity() const {return  MagCapacity;}
	FORCEINLINE float GetDamage() const {return  Damage;}
};
