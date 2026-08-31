#pragma once

#include "CoreMinimal.h"
#include "BuoyancyComponent.h"
#include "FROGGridShapeDefinition.h"
#include "GridComponent.h"
#include "PlaceableVisual.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Placeable.generated.h"

class UTimelineComponent;

UCLASS()
class CARGO_API APlaceable : public AActor
{
    GENERATED_BODY()
    
    friend class FPlaceableEditorDetails;   

protected:
    APlaceable();

    int32 GridLevel = -1;
    
    UPROPERTY(EditAnywhere, Category="Cargo")
    FFROGGridShapeDefinition GridShapeDefinition;    
	
    UPROPERTY(EditAnywhere, Category="Cargo")
    TObjectPtr<UContainerDA> PlaceableDA;   
    
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    TObjectPtr<UChildActorComponent> PlaceableVisualComp;

public:    
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UBoxComponent> BoxComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<USceneComponent> PivotComp;

    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    // TObjectPtr<UStaticMeshComponent> MeshComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    TObjectPtr<UBuoyancyComponent> BuoyancyComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
    float Weight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
    FString Name;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
    FGameplayTag PlaceableTag;

    UPROPERTY(EditDefaultsOnly, Category="Cargo")
    TObjectPtr<USoundBase> GrabSound;

    UPROPERTY(EditDefaultsOnly, Category="Cargo")
    TObjectPtr<USoundBase> PlaceSound;

    TObjectPtr<UGridComponent> OwningGridActor;    
    
    TObjectPtr<APlaceableVisual> GetVisual() { return Cast<APlaceableVisual>(PlaceableVisualComp->GetChildActor()); }

protected:

    UPROPERTY(VisibleAnywhere, Category = "Placement")
    float LocalYaw = 0.f;

    FIntVector PivotGridPos;

    virtual void BeginPlay() override;    
    
    void UpdateMesh();
public:

    virtual void Tick(float DeltaTime) override;
    
    virtual void OnConstruction(const FTransform& Transform) override;

    UFUNCTION(BlueprintCallable, Category = "Placement")
    float GetLocalYaw() const { return LocalYaw; }

    void Grab();

    void Place(TObjectPtr<UGridComponent> GridActor, int32 GridPosX, int32 GridPosY, int32 GridPosZ);
    
    void Release();

    void RotateClockwise();

    void AlignToRotation(const FRotator& ReferenceRotation);
    int32 GetPlaceableRotation();

    TArray<FVector> GetAllGridPositions(const FVector& BaseLocation, float Rotation, float CellSize) const;
    TArray<FIntVector> GetAllGridPositionsIndex(float Rotation) const;
    TArray<FIntVector> GetAllGridPositionsIndex(FIntVector GridPos, float Rotation);

    FIntVector GetGridPos() const { return PivotGridPos; }

    int32 GetGridLevel() const { return GridLevel; }

    UFUNCTION(BlueprintImplementableEvent, Category = "Placeable")
    void LaunchPlaceable(const FVector& Direction);    
    
    void FallIntoSea(const FVector& Direction);

    bool IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable);
    
    TObjectPtr<UContainerDA> GetPlaceableData() { return PlaceableDA; }
};