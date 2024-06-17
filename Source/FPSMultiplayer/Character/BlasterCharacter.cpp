// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "FPSMultiplayer/FPSMultiplayer.h"
#include "FPSMultiplayer/Components/CombatCompoment.h"
#include "Net/UnrealNetwork.h"
#include "FPSMultiplayer/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatCompoment>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f, 850.f);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
    NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
	HideMeshIfCharacterClip();
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquippeButtonP̦ressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPresses);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);
}
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}
void ABlasterCharacter::MoveForward(float Value)
{
	if(Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
	AddMovementInput(Direction, Value);
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquippeButtonP̦ressed()
{
	if(Combat)
	{
		if(HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();	
		}
	}
}
void ABlasterCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
	} else
	{
		Crouch();		
	}
}
void ABlasterCharacter::AimButtonPresses()
{
	if(Combat)
	{
		Combat->SetAiming(true);
	}
}
void ABlasterCharacter::AimButtonReleased()
{
	if(Combat)
	{
		Combat->SetAiming(false);
	}
}
void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon *Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}
bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}
bool ABlasterCharacter::IsAiming()
{
	return  (Combat && Combat->bAiming);
}
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(Combat && Combat->EquippedWeapon == nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if(Speed == 0.f && !bIsInAir)//Standing still
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(StartingAimRotation, CurrentAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.f || bIsInAir)//Run or jump
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{	//angle getting compressed to 0-360
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}
void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw,0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw)<15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}
AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if(Combat == nullptr)
		return  nullptr;
	return  Combat->EquippedWeapon;
}
void ABlasterCharacter::Jump()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}
void ABlasterCharacter::FireButtonPressed()
{
	if(Combat)
	{
		Combat->FireButtonPressed(true);
	}
}
void ABlasterCharacter::FireButtonReleased()
{
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
}
void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr)
		return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}
void ABlasterCharacter::PlayHitMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr)
		return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		SectionName = FName("Front");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::MulticastHit_Implementation()
{
	PlayHitMontage();
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}
void ABlasterCharacter::HideMeshIfCharacterClip()
{
	if(!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}








