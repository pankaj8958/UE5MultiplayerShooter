// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if(BlasterCharacter == nullptr)
    {
        BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if(BlasterCharacter == nullptr) return;
    FVector Velocity = BlasterCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();
    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
    bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    bIsCrouched = BlasterCharacter->bIsCrouched;
    bAiming = BlasterCharacter->IsAiming();

    //Yaw for straffing
    FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
    FRotator DeltaRotate = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRotate, DeltaTime, 6.f);
    YawOffset = DeltaRotation.Yaw;

    //For lean
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw/DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    AO_Yaw = BlasterCharacter->GetAOYaw();
    AO_Pitch = BlasterCharacter->GetAOPitch();
}
