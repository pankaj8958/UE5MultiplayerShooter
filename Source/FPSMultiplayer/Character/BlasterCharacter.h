// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "FPSMultiplayer/BlasterType/TurningInPlace.h"
#include "FPSMultiplayer/Interface/InteractWithCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "FPSMultiplayer/BlasterType/CombatState.h"
#include "FPSMultiplayer/GameMode/Team.h"
#include "FPSMultiplayer/Weapon/Weapon.h"
#include "BlasterCharacter.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnleftGame);
UCLASS()
class FPSMULTIPLAYER_API ABlasterCharacter : public ACharacter, public  IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void HideMeshIfCharacterClip();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	void Eliminate(bool bIsPlayerleft);
	UFUNCTION(NetMultiCast, Reliable)
	void MulticastEliminate(bool bIsPlayerleft);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();
	void SetTeamColor(ETeam Team);
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();
	FOnleftGame OnLeftGame;
	UPROPERTY(Replicated)
	bool bDisplayGameplay = false;
	virtual void Destroyed() override;
	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();
	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquippeButtonP̦ressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPresses();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAOPitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InsigatorController, AActor* DamageCauser);
	void PollInit();
	void RotateInPlace(float DeltaSecond);
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* head;
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditANywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatCompoment* PlayerCombat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	void PlayHitMontage();
	
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* ElimMontage;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_health, VisibleAnywhere, Category="Player Stats")
	float Health = 100.f;
	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_Shield, VisibleAnywhere, Category="Player Stats")
	float Shield = 100.f;
	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;
	bool bIsElim = false;
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	FTimerHandle ElimTimer;
	void ElimTimerFinished();
	
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(VisibleANywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterial;
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
	bool bLeftGame;
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;
	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* RedDissolveMatInst;
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* RedMaterial;
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* BlueDissolveMatInst;
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* BlueMaterial;
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* OriginalMaterial;
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAOYaw() const {return  AO_Yaw;}
	FORCEINLINE float GetAOPitch() const {return  AO_Pitch;}
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace(){return  TurningInPlace;}
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera(){return FollowCamera;}
	FORCEINLINE bool ShouldRotateRootBone(){return  bRotateRootBone;}
	FORCEINLINE bool IsElimmed(){return  bIsElim;}
	FORCEINLINE float GetHealth(){return  Health;}
	FORCEINLINE void SetHealth(float Amount) {Health = Amount;}
	FORCEINLINE float GetMaxHealth(){return  MaxHealth;}
	FORCEINLINE float GetShild(){return  Shield;}
	FORCEINLINE float GetMaxShield(){return MaxShield;}
	ECombatType GetCombatState() const;
	FORCEINLINE UCombatCompoment* GetCombat() const {return  PlayerCombat;}
	FORCEINLINE bool GetDisableGameplay() const {return  bDisplayGameplay;}
	FORCEINLINE UBuffComponent* GetBuff() const {return  Buff;}
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const{return  LagCompensation;}
};
