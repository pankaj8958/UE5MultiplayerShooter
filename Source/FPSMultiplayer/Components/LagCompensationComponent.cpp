#include "LagCompensationComponent.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : Character;
	if(Character)
	{
		Package.time = GetWorld()->GetTimeSeconds();
		for (auto& Boxpair : Character->HitCollisionBoxes)
		{
			FBoxInfo BoxInformation;
			BoxInformation.Location = Boxpair.Value->GetComponentLocation();
			BoxInformation.Rotation = Boxpair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = Boxpair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(Boxpair.Key, BoxInformation);
		}
	}
}
void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat( BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}
void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(FrameHistory.Num() <= 1)
	{
		FFramePackage ThePackage;
		SaveFramePackage(ThePackage);
		FrameHistory.AddHead(ThePackage);
	} else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().time - FrameHistory.GetTail()->GetValue().time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().time - FrameHistory.GetTail()->GetValue().time;
		}
		FFramePackage ThePackage;
		SaveFramePackage(ThePackage);
		FrameHistory.AddHead(ThePackage);
		ShowFramePackage(ThePackage, FColor::Blue);
	}
}


