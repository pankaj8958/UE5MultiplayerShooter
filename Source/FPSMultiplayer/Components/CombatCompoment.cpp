// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCompoment.h"
#include "Camera/CameraComponent.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "FPSMultiplayer/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FPSMultiplayer/PlayerController/BlasterPlayerController.h"
#include "FPSMultiplayer/Widget/BlasterHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include  "FPSMultiplayer/Interface/InteractWithCrosshairInterface.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
UCombatCompoment::UCombatCompoment()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}
// Called when the game starts
void UCombatCompoment::BeginPlay()
{
	Super::BeginPlay();
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}
// Called every frame
void UCombatCompoment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrossHairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}
void UCombatCompoment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatCompoment, EquippedWeapon);
	DOREPLIFETIME(UCombatCompoment, bAiming);
}
void UCombatCompoment::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}
void UCombatCompoment::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}
void UCombatCompoment::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr)
		return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}
void UCombatCompoment::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}
void UCombatCompoment::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrossHairs(HitResult);
		ServerFire(HitResult.ImpactPoint);

		if(EquippedWeapon)
		{
			CrosshairShootFactor = 0.75f;
		}
	}
}
void UCombatCompoment::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}
void UCombatCompoment::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon == nullptr)
		return;
	if(Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}
void UCombatCompoment::TraceUnderCrossHairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X/2.f, ViewportSize.Y/2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if(Character)
		{
			float DistanceBetweenCamera = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceBetweenCamera  + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * 80000.f;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
			);
		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
				);
		}
	}
}

void UCombatCompoment::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr)
		return;;
	PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
	if(PlayerController)
	{
		HUD = HUD == nullptr? Cast<ABlasterHUD>(PlayerController->GetHUD()) : HUD;
		if(HUD)
		{
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairCentre = EquippedWeapon->CrossHairsCentre;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrossHairsLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrossHairsRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrossHairsTop;
				HUDPackage.CrosshairBottom = EquippedWeapon->CrossHairsBottom;
				
			}
			else
			{
				
				HUDPackage.CrosshairCentre = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}
			//Crosshair spread or shrink
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			//Speen and target value can be configurable
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			} else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
			if(bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);			
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}
			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 40.f);
			
			HUDPackage.CrosshairSpread =
			0.58 +//Padding value
			CrosshairVelocityFactor -
			CrosshairAimFactor +
			CrosshairInAirFactor +
			CrosshairShootFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}
void UCombatCompoment::InterpFOV(float Deltatime)
{
	if(EquippedWeapon == nullptr) return;
	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), Deltatime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, Deltatime, EquippedWeapon->GetZoomInterpSpeed());
	}
	if(Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}








