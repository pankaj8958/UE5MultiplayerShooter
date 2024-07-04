// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSMultiplayer/BlasterType/TurningInPlace.h"
#include "FPSMultiplayer/Interface/InteractWithCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

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
	virtual void OnRep_ReplicatedMovement() override;
	void HideMeshIfCharacterClip();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	void Eliminate();
	UFUNCTION(NetMultiCast, Reliable)
	void MulticastEliminate();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquippeButtonP̦ressed();
	void CrouchButtonPressed();
	void AimButtonPresses();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAOPitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InsigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	
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

	UPROPERTY(VisibleAnywhere)
	class UCombatCompoment* Combat;

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
	void OnRep_Health();

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
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
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
	FORCEINLINE float GetMaxHealth(){return  MaxHealth;}
};
