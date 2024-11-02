// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252
UENUM(BlueprintType)
enum class EWeaponType:uint8
{
    EWT_AssaultRifle UMETA(Display = "Assault Rifle"),
    EWT_RocketLauncher UMETA(Display = "Rocket Launcher"),
    EWT_Pistol UMETA(Display = "Pistol"),
    EWT_Flag UMETA(DisplayName = "Flag"),
    EWT_MAX UMETA(DisplayType = "Default Max")
};
