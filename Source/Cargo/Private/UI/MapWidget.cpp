#include "UI/MapWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Island/CargoIsland.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

class UCanvasPanelSlot;

void UMapWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetupMap();
}

void UMapWidget::NativeDestruct()
{
    TeardownMap();
    Super::NativeDestruct();
}

void UMapWidget::SetupMap()
{
    UpdateInterestPlaces();

    // Player marker não precisa de framerate de render; timer é suficiente
    // e para automaticamente quando o widget fecha.
    GetWorld()->GetTimerManager().SetTimer(
        PlayerMarkerTimerHandle,
        this,
        &UMapWidget::UpdatePlayerMarker,
        PlayerMarkerUpdateInterval, // ex: 0.1f (10Hz), exposto como UPROPERTY se quiser tunar
        true // loop
    );

    // Atualiza uma vez de imediato, sem esperar o primeiro tick do timer.
    UpdatePlayerMarker();
}

void UMapWidget::TeardownMap()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(PlayerMarkerTimerHandle);
    }

    for (const auto& Widget : MapInterestPlaceWidgets)
    {
        if (Widget)
        {
            Widget->RemoveFromParent();
        }
    }
    MapInterestPlaceWidgets.Empty();
}

void UMapWidget::UpdateInterestPlaces()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACargoIsland::StaticClass(), FoundActors);

    for (const auto Actor : FoundActors)
    {
        const auto Island = Cast<ACargoIsland>(Actor);
        if (!Island)
        {
            continue;
        }

        auto MapInterestWidget = CreateWidget<UMapInterestPlaceWidget>(GetWorld(), MapInterestPlaceClass);
        MapInterestWidget->Init(IslandIcon, Island->GetIslandName());
        MapInterestWidget->SetVisibility(ESlateVisibility::Visible);

        UCanvasPanelSlot* PanelSlot = MapCanvas->AddChildToCanvas(MapInterestWidget);
        SetSlotAnchorFromWorldPosition(PanelSlot, Island->GetActorLocation());

        MapInterestPlaceWidgets.Add(MapInterestWidget);
    }
}

void UMapWidget::UpdatePlayerMarker()
{
    const APawn* OwningPawn = GetOwningPlayerPawn();
    if (!OwningPawn || !PlayerImage)
    {
        return;
    }

    UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(PlayerImage->Slot);
    if (!PanelSlot)
    {
        return;
    }

    SetSlotAnchorFromWorldPosition(PanelSlot, OwningPawn->GetActorLocation());
    PlayerImage->SetRenderTransformAngle(OwningPawn->GetActorRotation().Yaw);
}

void UMapWidget::SetSlotAnchorFromWorldPosition(UCanvasPanelSlot* PanelSlot, const FVector& WorldLocation) const
{
    if (!PanelSlot)
    {
        return;
    }

    const FVector2D Anchor = WorldToMapAnchor(WorldLocation);

    PanelSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y, Anchor.X, Anchor.Y));
    PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // centraliza o ícone no ponto exato
    PanelSlot->SetPosition(FVector2D::ZeroVector);
}

FVector2D UMapWidget::WorldToMapAnchor(const FVector& WorldLocation) const
{
    // Posição do ponto relativa ao centro do capture do mapa.
    const FVector2D Delta(WorldLocation.X - MapCaptureCenter.X, WorldLocation.Y - MapCaptureCenter.Y);

    const float HalfMapSize = MapCaptureOrthoWidth * 0.5f;

    // Normaliza para [-1, 1].
    const float NormalizedX = FMath::Clamp(Delta.X / HalfMapSize, -1.f, 1.f);
    const float NormalizedY = FMath::Clamp(Delta.Y / HalfMapSize, -1.f, 1.f);

    // Mesma troca de eixo/sinal que você já tinha, só que aplicada antes do remap.
    const float SwappedX = NormalizedY;
    const float SwappedY = -NormalizedX;

    // Remapeia de [-1, 1] para [0, 1] (formato esperado por AnchorMin/AnchorMax).
    const float AnchorX = (SwappedX + 1.f) * 0.5f;
    const float AnchorY = (SwappedY + 1.f) * 0.5f;

    return FVector2D(AnchorX, AnchorY);
}