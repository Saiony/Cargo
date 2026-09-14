#include "Interaction/PortBellInteractable.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

APortBellInteractable::APortBellInteractable()
{
	BellMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BellMesh"));
	BellMesh->SetupAttachment(InteractionVisualRoot);
}

void APortBellInteractable::PlayInteractionAnimation()
{
	if (!ClickMontage || !BellMesh)
		return;

	BellMesh->PlayAnimation(ClickMontage, false);
}

void APortBellInteractable::Interact()
{
	OnBellRung.Broadcast();
}
