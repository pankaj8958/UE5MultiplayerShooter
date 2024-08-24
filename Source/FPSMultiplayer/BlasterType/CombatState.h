#pragma  once
UENUM(BlueprintType)
enum class ECombatType:uint8
{
    ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
    ECS_Reloading UMETA(DisplayName = "Reloading"),
    ECS_Granade UMETA(DisplayName = "Granade"),
    ECS_MAX UMETA(DisplayName = "DefaultMax")
};