// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRump(DeltaTime);
}
void UBuffComponent::Heal(float HealAmt, float HealTime)
{
	bHealing = true;
	HealingRate = HealAmt/HealTime;
	AmountToHeal += HealAmt;
}

void UBuffComponent::SetInitialSpeed(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}
void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if(Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeed,
		BuffTime
	);

	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::HealRump(float DeltaTime)
{
	if(!bHealing || Character == nullptr || Character->IsElimmed())
		return;
	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	if(AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ResetSpeed()
{
	if(Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

