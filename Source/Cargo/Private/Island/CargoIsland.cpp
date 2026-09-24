// Fill out your copyright notice in the Description page of Project Settings.


#include "Island/CargoIsland.h"

#include "PrimaryGameLayout.h"
#include "Components/WidgetComponent.h"
#include "TagDeclaration/UITypes.h"
#include "UI/Island/IslandWidget.h"

ACargoIsland::ACargoIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	IslandMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
	IslandMeshComp->SetupAttachment(RootComponent);

	PortComponent = CreateDefaultSubobject<UCargoPortComponent>(TEXT("PortComponent"));
	PortComponent->SetupAttachment(RootComponent);
	
	InteractableWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidgetComp"));
	InteractableWidgetComp->SetupAttachment(RootComponent);
}

void ACargoIsland::BeginPlay()
{
	Super::BeginPlay();

	const auto GM = ACargoGameMode::Get(this);
	
	const auto QuestService = GM->QuestService;
	QuestService->MissionAcceptedDelegate.AddUObject(this, &ThisClass::OnMissionAccepted);
	QuestService->MissionCompletedDelegate.AddUObject(this, &ThisClass::OnMissionCompleted);
	
	Unfocus();
}

void ACargoIsland::Interact_Implementation(AActor* Interactor)
{	
	UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());

	const auto PrimaryGameLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	const auto IslandWidget = PrimaryGameLayout->PushWidgetToLayerStack<UIslandWidget>(TAG_UI_Layer_GameMenu, IslandWidgetClass.LoadSynchronous());
	
	IslandWidget->Initialize(this);
}

void ACargoIsland::Focus()
{
	InteractableWidgetComp->SetVisibility(true);
}

void ACargoIsland::Unfocus()
{
	InteractableWidgetComp->SetVisibility(false);	
}

void ACargoIsland::OnMissionAccepted(TObjectPtr<UDeliveryMissionStatus> MissionStatus, FGameplayTag InstigatorIslandTag)
{
	if (InstigatorIslandTag != GetLocationTag())
		return;
	
	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Mission accepted but PortComponent is missing!"), *GetLocationTag().ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Mission accepted! Spawning containers at PortComponent..."), *GetLocationTag().ToString());
	
	PortComponent->SpawnCargo(MissionStatus->GetOriginalMissionData()->CargoRequirements);
}

void ACargoIsland::OnMissionCompleted(TObjectPtr<UDeliveryMissionStatus> MissionStatus)
{
	if (MissionStatus->GetDestinationTag() != GetLocationTag())
		return;
	
	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Mission accepted but PortComponent is missing!"), *GetLocationTag().ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Mission completed!"), *GetLocationTag().ToString());
	
	PortComponent->Clear();
}
