// Fill out your copyright notice in the Description page of Project Settings.

#include "Debug/CargoDebugCommands.h"
#include "CargoCharacter.h"
#include "CargoPlayerController.h"
#include "DeveloperSettings/CargoSettings.h"
#include "DataAssets/ContainerDA.h"
#include "Grid/Container.h"
#include "Grid/GridComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

static FAutoConsoleCommandWithWorldAndArgs CmdCargoLoadShip(
	TEXT("Cargo.LoadShip"),
	TEXT("Loads one level of containers onto the ship. Usage: Cargo.LoadShip [CargoType]"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&UCargoDebugCommands::LoadShipConsole)
);

static FAutoConsoleCommandWithWorldAndArgs CmdCargoGetContainer(
	TEXT("Cargo.GetContainer"),
	TEXT("Spawns a container in hand (dragging mode). Usage: Cargo.GetContainer [CargoType]"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&UCargoDebugCommands::GetContainerConsole)
);

UContainerDA* UCargoDebugCommands::ResolveContainerDataAsset(const FString& InCargoTypeStr, FGameplayTag* OutTag)
{
	const UCargoSettings* Settings = GetDefault<UCargoSettings>();
	if (!Settings || Settings->ContainersMap.IsEmpty())
	{
		return nullptr;
	}

	FString CleanStr = InCargoTypeStr.TrimStartAndEnd().TrimQuotes();
	CleanStr.RemoveFromStart(TEXT("("));
	CleanStr.RemoveFromEnd(TEXT(")"));
	CleanStr.TrimStartAndEndInline();
	CleanStr = CleanStr.TrimQuotes();

	if (CleanStr.IsEmpty())
	{
		for (const auto& Pair : Settings->ContainersMap)
		{
			if (Pair.Value.IsNull())
			{
				continue;
			}

			if (UContainerDA* LoadedDA = Pair.Value.LoadSynchronous())
			{
				if (OutTag)
				{
					*OutTag = Pair.Key;
				}
				return LoadedDA;
			}
		}
		return nullptr;
	}

	FGameplayTag RequestedTag = FGameplayTag::RequestGameplayTag(FName(*CleanStr), false);
	if (RequestedTag.IsValid())
	{
		if (const TSoftObjectPtr<UContainerDA>* FoundPtr = Settings->ContainersMap.Find(RequestedTag))
		{
			if (!FoundPtr->IsNull())
			{
				if (UContainerDA* LoadedDA = FoundPtr->LoadSynchronous())
				{
					if (OutTag)
					{
						*OutTag = RequestedTag;
					}
					return LoadedDA;
				}
			}
		}
	}

	if (!CleanStr.StartsWith(TEXT("Cargo.")))
	{
		const FString Prefixed = FString::Printf(TEXT("Cargo.%s"), *CleanStr);
		const FGameplayTag PrefixedTag = FGameplayTag::RequestGameplayTag(FName(*Prefixed), false);
		if (PrefixedTag.IsValid())
		{
			if (const TSoftObjectPtr<UContainerDA>* FoundPtr = Settings->ContainersMap.Find(PrefixedTag))
			{
				if (!FoundPtr->IsNull())
				{
					if (UContainerDA* LoadedDA = FoundPtr->LoadSynchronous())
					{
						if (OutTag)
						{
							*OutTag = PrefixedTag;
						}
						return LoadedDA;
					}
				}
			}
		}
	}

	for (const auto& Pair : Settings->ContainersMap)
	{
		const FString TagNameStr = Pair.Key.ToString();
		const FString AssetNameStr = Pair.Value.GetAssetName();

		if (TagNameStr.Equals(CleanStr, ESearchCase::IgnoreCase) ||
			TagNameStr.EndsWith(CleanStr, ESearchCase::IgnoreCase) ||
			AssetNameStr.Equals(CleanStr, ESearchCase::IgnoreCase) ||
			AssetNameStr.Contains(CleanStr, ESearchCase::IgnoreCase))
		{
			if (!Pair.Value.IsNull())
			{
				if (UContainerDA* LoadedDA = Pair.Value.LoadSynchronous())
				{
					if (OutTag)
					{
						*OutTag = Pair.Key;
					}
					return LoadedDA;
				}
			}
		}
	}

	return nullptr;
}

void UCargoDebugCommands::LoadShip(UObject* WorldContextObject, const FString& CargoType)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	FGameplayTag CargoTag;
	UContainerDA* ContainerDA = ResolveContainerDataAsset(CargoType, &CargoTag);
	if (!ContainerDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.LoadShip: Could not find valid container for '%s'"), *CargoType);
		return;
	}

	ACargoCharacter* Ship = nullptr;
	const APlayerController* PC = World->GetFirstPlayerController();
	if (PC)
	{
		Ship = Cast<ACargoCharacter>(PC->GetPawn());
	}

	if (!Ship)
	{
		for (TActorIterator<ACargoCharacter> It(World); It; ++It)
		{
			Ship = *It;
			break;
		}
	}

	if (!Ship)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.LoadShip: Ship (ACargoCharacter) not found in world"));
		return;
	}

	UGridComponent* GridComp = Ship->FindComponentByClass<UGridComponent>();
	if (!GridComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.LoadShip: GridComponent not found on Ship"));
		return;
	}

	const int32 TargetZ = GridComp->GetOccupiedSlots().IsEmpty() ? 0 : (GridComp->GetHighestOccupiedZ() + 1);
	const FIntVector Min = GridComp->GetMin();
	const FIntVector Max = GridComp->GetMax();
	const float CellSize = GridComp->GetCellSize();

	const UCargoSettings* Settings = GetDefault<UCargoSettings>();
	const TSubclassOf<AContainer> ContainerClass = (Settings && Settings->ContainerClass) ? Settings->ContainerClass : TSubclassOf<AContainer>(AContainer::StaticClass());

	int32 PlacedCount = 0;

	for (int32 X = Min.X; X <= Max.X; ++X)
	{
		for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
		{
			const FVector LocalPos(X * CellSize, Y * CellSize, TargetZ * CellSize);
			const FVector WorldPos = GridComp->GetComponentTransform().TransformPosition(LocalPos);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Ship;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AContainer* NewContainer = World->SpawnActor<AContainer>(ContainerClass, WorldPos, GridComp->GetComponentRotation(), SpawnParams);
			if (!NewContainer)
			{
				continue;
			}

			NewContainer->Init(ContainerDA);

			if (GridComp->CanAddPlaceableToGrid(NewContainer, WorldPos, 0.0f))
			{
				NewContainer->SetActorLocationAndRotation(WorldPos, GridComp->GetComponentRotation());
				NewContainer->AttachToComponent(GridComp, FAttachmentTransformRules::KeepWorldTransform);
				GridComp->AddPlaceableToGrid(NewContainer, WorldPos, 0.0f);
				PlacedCount++;
			}
			else
			{
				NewContainer->Destroy();
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Cargo.LoadShip: Placed %d '%s' containers on level %d"), PlacedCount, *ContainerDA->Name, TargetZ);
}

void UCargoDebugCommands::GetContainer(UObject* WorldContextObject, const FString& CargoType)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	FGameplayTag CargoTag;
	UContainerDA* ContainerDA = ResolveContainerDataAsset(CargoType, &CargoTag);
	if (!ContainerDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.GetContainer: Could not find valid container for '%s'"), *CargoType);
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	ACargoPlayerController* CargoPC = Cast<ACargoPlayerController>(PC);
	if (!CargoPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.GetContainer: ACargoPlayerController not found"));
		return;
	}

	APawn* ControlledPawn = CargoPC->GetPawn();
	const FVector SpawnLoc = ControlledPawn ? (ControlledPawn->GetActorLocation() + FVector(0.f, 0.f, 200.f)) : FVector::ZeroVector;
	const FRotator SpawnRot = ControlledPawn ? ControlledPawn->GetActorRotation() : FRotator::ZeroRotator;

	const UCargoSettings* Settings = GetDefault<UCargoSettings>();
	const TSubclassOf<AContainer> ContainerClass = (Settings && Settings->ContainerClass) ? Settings->ContainerClass : TSubclassOf<AContainer>(AContainer::StaticClass());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ControlledPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AContainer* NewContainer = World->SpawnActor<AContainer>(ContainerClass, SpawnLoc, SpawnRot, SpawnParams);
	if (!NewContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo.GetContainer: Failed to spawn container"));
		return;
	}

	NewContainer->Init(ContainerDA);
	CargoPC->StartDragging(NewContainer);

	UE_LOG(LogTemp, Log, TEXT("Cargo.GetContainer: Grabbed '%s' container"), *ContainerDA->Name);
}

void UCargoDebugCommands::LoadShipConsole(const TArray<FString>& Args, UWorld* World)
{
	const FString CargoType = Args.Num() > 0 ? Args[0] : FString();
	LoadShip(World, CargoType);
}

void UCargoDebugCommands::GetContainerConsole(const TArray<FString>& Args, UWorld* World)
{
	const FString CargoType = Args.Num() > 0 ? Args[0] : FString();
	GetContainer(World, CargoType);
}
