#include "Interaction/WorldInteractable.h"

#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

AWorldInteractable::AWorldInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	InteractionVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("InteractionVisualRoot"));
	InteractionVisualRoot->SetupAttachment(SceneRoot);

	InteractionHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionHitbox"));
	InteractionHitbox->SetupAttachment(SceneRoot);
	InteractionHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionHitbox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AWorldInteractable::BeginPlay()
{
	Super::BeginPlay();

	InteractionVisuals.Reset();

	TArray<USceneComponent*> VisualChildren;
	InteractionVisualRoot->GetChildrenComponents(true, VisualChildren);
	for (USceneComponent* Child : VisualChildren)
	{
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Child))
		{
			InteractionVisuals.Add(Primitive);
		}
	}

	if (!ensureMsgf(IsValid(InteractionHitbox), TEXT("%s has no valid InteractionHitbox"), *GetName()))
	{
		return;
	}

	InteractionHitbox->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleBeginCursorOver);
	InteractionHitbox->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleEndCursorOver);
	InteractionHitbox->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void AWorldInteractable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(InteractionHitbox))
	{
		InteractionHitbox->OnBeginCursorOver.RemoveDynamic(this, &ThisClass::HandleBeginCursorOver);
		InteractionHitbox->OnEndCursorOver.RemoveDynamic(this, &ThisClass::HandleEndCursorOver);
		InteractionHitbox->OnClicked.RemoveDynamic(this, &ThisClass::HandleClicked);
	}

	for (UPrimitiveComponent* Visual : InteractionVisuals)
	{
		if (Visual)
		{
			Visual->SetRenderCustomDepth(false);
		}
	}
	InteractionVisuals.Reset();

	Super::EndPlay(EndPlayReason);
}

void AWorldInteractable::PlayInteractionAnimation()
{
}

void AWorldInteractable::Interact()
{
}

void AWorldInteractable::HandleBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (TouchedComponent != InteractionHitbox)
	{
		return;
	}

	for (UPrimitiveComponent* Visual : InteractionVisuals)
	{
		if (Visual)
		{
			Visual->SetCustomDepthStencilValue(CustomDepthStencilValue);
			Visual->SetRenderCustomDepth(true);
		}
	}

	if (HoverSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HoverSound, GetActorLocation());
	}
}

void AWorldInteractable::HandleEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (TouchedComponent != InteractionHitbox)
	{
		return;
	}

	for (UPrimitiveComponent* Visual : InteractionVisuals)
	{
		if (Visual)
		{
			Visual->SetRenderCustomDepth(false);
		}
	}
}

void AWorldInteractable::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (TouchedComponent != InteractionHitbox)
	{
		return;
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ClickSound, GetActorLocation());
	}

	PlayInteractionAnimation();
	Interact();
}
