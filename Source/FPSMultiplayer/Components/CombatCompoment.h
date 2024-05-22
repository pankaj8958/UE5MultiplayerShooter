// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatCompoment.generated.h"

class AWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSMULTIPLAYER_API UCombatCompoment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatCompoment();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend  class ABlasterCharacter;
	void EquipWeapon(AWeapon* WeaponToEquip);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	class ABlasterCharacter* Character;
	class AWeapon* EquippedWeapon;
};
