// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EWeaponType:uint8
{
    EWT_AssaultRifle UMETA(Display = "Assault Rifle"),
    
    EWT_MAX UMETA(DisplayType = "Default Max")
};
