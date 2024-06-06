#pragma  once
UENUM(BlueprintType)
enum  class ETurningInPlace:uint8
{
	ETIP_Left UMETA(DisplaName = "Turning Left"),
	ETIP_Right UMETA(DisplaName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplaName = "Not Turning"),
	
	ETIP_MAX UMETA(DisplayName = "DefaultMax")
};
