#pragma once

#include "CoreMinimal.h"
#include "Interaction/WorldInteractable.h"
#include "PortBellInteractable.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBellRung);

UCLASS(Blueprintable)
class CARGO_API APortBellInteractable : public AWorldInteractable
{
	GENERATED_BODY()

public:
	APortBellInteractable();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnBellRung OnBellRung;

protected:
	virtual void PlayInteractionAnimation() override;
	virtual void Interact() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> BellMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Animation")
	TObjectPtr<UAnimMontage> ClickMontage;
};
