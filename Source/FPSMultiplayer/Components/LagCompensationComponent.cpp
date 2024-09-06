#include "LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "FPSMultiplayer/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}
FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if(HitCharacter == nullptr) return  FServerSideRewindResult();
	FFramePackage CurrentTime;
	CacheBoxPosition(HitCharacter, CurrentTime);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::Type::NoCollision);

	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart* (HitLocation - TraceStart) * 1.2f;
	UWorld* World = GetWorld();
	if(World)
	{
		World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility
		);
		if(ConfirmHitResult.bBlockingHit)
		{
			ResetHitBoxes(HitCharacter, CurrentTime);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::Type::QueryAndPhysics);
			return  FServerSideRewindResult{true, true};
		}
		else
		{
			for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
			{
				if(HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
				}
			}
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_Visibility
			);
			if(ConfirmHitResult.bBlockingHit)
			{
				ResetHitBoxes(HitCharacter, CurrentTime);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::Type::QueryAndPhysics);
				return FServerSideRewindResult{true, false};
			}
		}
	}
	ResetHitBoxes(HitCharacter, CurrentTime);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::Type::QueryAndPhysics);
	return FServerSideRewindResult{false, false};
}
void ULagCompensationComponent::CacheBoxPosition(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if(HitCharacter == nullptr) return;
	for (auto& HitBoxPair :  HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value!=nullptr)
		{
			FBoxInfo BoxInfo;
			BoxInfo.Location=HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation=HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent=HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
		}
	}
}
void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for (auto& HitBoxPair :  HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value!=nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		}
	}
}
void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for (auto& HitBoxPair :  HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value!=nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		}
	}
}
void ULagCompensationComponent::EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled)
{
	if(HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
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

FFramePackage ULagCompensationComponent::InterpolateBetweenFrames(const FFramePackage& OlderFrame,
	const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.time - OlderFrame.time;
	const float InterpoFraction = FMath::Clamp((HitTime - OlderFrame.time)/Distance,0.f, 1.f);
	FFramePackage InterpolateFramePackage;
	InterpolateFramePackage.time = HitTime;
	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxName = YoungerPair.Key;
		const FBoxInfo& OlderBox = OlderFrame.HitBoxInfo[BoxName];
		const FBoxInfo& YoungBox = YoungerFrame.HitBoxInfo[BoxName];
		FBoxInfo InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungBox.Location, 1.f, InterpoFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungBox.Rotation, 1.f, InterpoFraction);
		InterpBoxInfo.BoxExtent = YoungBox.BoxExtent;
		InterpolateFramePackage.HitBoxInfo.Add(BoxName, InterpBoxInfo);
	}
	return  InterpolateFramePackage;
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

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation, float HitTime)
{
	bool bReturn = HitCharacter==nullptr||
			HitCharacter->GetLagCompensation() == nullptr ||
			HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
			HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
	if(bReturn) return FServerSideRewindResult();
	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistory = History.GetTail()->GetValue().time;
	const float NewestHistory = History.GetHead()->GetValue().time;
	if(OldestHistory > HitTime)
	{
		return FServerSideRewindResult();
	}
	if(OldestHistory == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if(NewestHistory <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
	while (Older->GetValue().time > HitTime)
	{
		if(Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();
		if(Older->GetValue().time > HitTime)
		{
			Younger = Older;
		}
	}
	if(Older->GetValue().time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}
	if(bShouldInterpolate)
	{
		FrameToCheck = InterpolateBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);
	if(Character && HitCharacter && DamageCauser && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			DamageCauser->GetDamage(),
			Character->Controller,
			DamageCauser,
			UDamageType::StaticClass()
		);
	}
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SaveFramePackage();
}
void ULagCompensationComponent::SaveFramePackage()
{
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
		//ShowFramePackage(ThePackage, FColor::Blue);
	}
}



