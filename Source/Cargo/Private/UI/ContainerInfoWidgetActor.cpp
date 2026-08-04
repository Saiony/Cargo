// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContainerInfoWidgetActor.h"

#include "CargoPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ContainerInfoWidget.h"


// Sets default values
AContainerInfoWidgetActor::AContainerInfoWidgetActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AContainerInfoWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	Hide();
	
	const auto PlayerController = Cast<ACargoPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PlayerController->OnContainerHoverConfirmed.AddDynamic(this, &ThisClass::HandleContainerHoverConfirmed);
}

void AContainerInfoWidgetActor::HandleContainerHoverConfirmed(AContainer* HoveredContainer)
{
	if (HoveredContainer == nullptr)
	{
		Hide();
		return;
	}
	
	WidgetComponent->SetVisibility(true);
	WidgetComponent->SetHiddenInGame(false);
	
	SetActorLocation(HoveredContainer->GetActorLocation() + Offset);
	
	const auto Widget = Cast<UContainerInfoWidget>(WidgetComponent->GetUserWidgetObject());
	Widget->Show(HoveredContainer->Name, HoveredContainer->Weight);
}

void AContainerInfoWidgetActor::Hide()
{
	WidgetComponent->SetHiddenInGame(true);
	WidgetComponent->SetVisibility(false);
}

// Called every frame
void AContainerInfoWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

