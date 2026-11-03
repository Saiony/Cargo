// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Specifics/SingleNPC.h"

#include "CargoGameMode.h"
#include "Components/WidgetComponent.h"
#include "Subsystem/FROGDialogueSubsystem.h"

ASingleNPC::ASingleNPC()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InteractableWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidgetComp"));
	InteractableWidgetComp->SetupAttachment(RootComponent);
}

void ASingleNPC::BeginPlay()
{
	Super::BeginPlay();
}

void ASingleNPC::Focus()
{
	InteractableWidgetComp->SetVisibility(true);
}

void ASingleNPC::Unfocus()
{
	InteractableWidgetComp->SetVisibility(false);
}

void ASingleNPC::Interact_Implementation(AActor* Interactor)
{
	ICargoInteractable::Interact_Implementation(Interactor);
	
	const auto DialogueSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
	DialogueSubsystem->PlayDialogue(DialogueData, this);
}
