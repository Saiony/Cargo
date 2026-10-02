// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/UnlockableObstacle.h"

#include "PrimaryGameLayout.h"
#include "Components/WidgetComponent.h"
#include "TagDeclaration/UITypes.h"


// Sets default values
AUnlockableObstacle::AUnlockableObstacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ObstacleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMeshComp"));
	SetRootComponent(ObstacleMeshComp);
	
	InteractableWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidgetComp"));
	InteractableWidgetComp->SetupAttachment(RootComponent);
	
	InteractableWidgetComp->SetVisibility(false);		
}

// Called when the game starts or when spawned
void AUnlockableObstacle::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AUnlockableObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUnlockableObstacle::Focus()
{
	InteractableWidgetComp->SetVisibility(true);
}

void AUnlockableObstacle::Unfocus()
{
	InteractableWidgetComp->SetVisibility(false);
}

void AUnlockableObstacle::Interact_Implementation(AActor* Interactor)
{
	ICargoInteractable::Interact_Implementation(Interactor);
	
	const auto PrimaryGameLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	const auto IslandWidget = PrimaryGameLayout->PushWidgetToLayerStack<USimplePurchaseWidget>(TAG_UI_Layer_GameMenu, SimplePurchaseWidgetClass);
	
	IslandWidget->Initialize(CostToUnlock, "Desbloquear obstaculo por:", this);
}

void AUnlockableObstacle::OnSimplePurchaseWidgetCallback(bool Purchased)
{
	if (Purchased)
		Unlock();
}

void AUnlockableObstacle::Unlock()
{
	Destroy();
}
