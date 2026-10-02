#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerEnterTrigger.generated.h"

class UBoxComponent;

UCLASS()
class CARGO_API APlayerEnterTrigger : public AActor
{
	GENERATED_BODY()

public:
	APlayerEnterTrigger();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UBoxComponent> Box;

	virtual void OnPlayerEnter() {}

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
