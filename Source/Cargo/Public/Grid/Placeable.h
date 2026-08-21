#pragma once

#include "CoreMinimal.h"
#include "BuoyancyComponent.h"
#include "GridComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Placeable.generated.h"

class UTimelineComponent;

UCLASS()
class CARGO_API APlaceable : public AActor
{
    GENERATED_BODY()

protected:
    APlaceable();

    int32 GridLevel = -1;

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UBoxComponent> BoxComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<USceneComponent> PivotComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UStaticMeshComponent> MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UMaterialInterface> Material;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UBuoyancyComponent> BuoyancyComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
    float Weight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    FString Name;

    UPROPERTY(EditDefaultsOnly, Category="Cargo")
    TObjectPtr<USoundBase> GrabSound;

    UPROPERTY(EditDefaultsOnly, Category="Cargo")
    TObjectPtr<USoundBase> PlaceSound;

    TObjectPtr<UGridComponent> OwningGridActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
    FVector2D Size;

protected:

    UPROPERTY(VisibleAnywhere, Category = "Placement")
    float LocalYaw = 0.f;

    FIntPoint PivotGridPos;

    virtual void BeginPlay() override;

public:

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Placement")
    float GetLocalYaw() const { return LocalYaw; }

    void Grab();

    void Place(TObjectPtr<UGridComponent> GridActor, int32 GridPosX, int32 GridPosY, int32 GridPosZ);
    
    void Release();

    void RotateClockwise();

    void AlignToRotation(const FRotator& ReferenceRotation);

    TArray<FVector> GetAllGridPositions(const FVector& BaseLocation, float Rotation, float CellSize) const;

    FIntPoint GetGridPos() const { return PivotGridPos; }

    int32 GetGridLevel() const { return GridLevel; }

    UFUNCTION(BlueprintImplementableEvent, Category = "Placeable")
    void LaunchPlaceable(const FVector& Direction);

    bool IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable);
    
};