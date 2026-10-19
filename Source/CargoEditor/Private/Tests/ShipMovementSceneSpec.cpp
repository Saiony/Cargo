// Este Spec usa APIs do Editor para abrir o mapa e iniciar uma sessao de PIE.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "CargoCharacter.h"
#include "CargoPlayerController.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformTime.h"
#include "Tests/AutomationEditorCommon.h"
#include "Containers/Ticker.h"

// Como este teste carrega uma fase e roda o jogo, ele e um teste de integracao.
BEGIN_DEFINE_SPEC(FShipMovementSceneSpec,
	"Cargo.Scene.ShipMovement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FShipMovementSceneSpec)

void FShipMovementSceneSpec::Define()
{
	Describe("Movimento do barco no mapa principal", [this]()
	{
		LatentIt("anda depois de receber movimento por cinco segundos", FTimespan::FromSeconds(45),
			[this](const FDoneDelegate& Done)
		{
			// Abrimos o mapa configurado como mapa principal do jogo.
			FAutomationEditorCommonUtils::LoadMap(TEXT("/Game/Cargo/L_Main"));

			// Solicitamos Play In Editor para que movimento e fisica avancem de verdade.
			FRequestPlaySessionParams PlayParams;
			PlayParams.WorldType = EPlaySessionWorldType::PlayInEditor;
			PlayParams.GlobalMapOverride = TEXT("/Game/Cargo/L_Main");
			GEditor->RequestPlaySession(PlayParams);

			// Guardamos o estado entre os frames enquanto o mapa inicia e o barco se move.
			const double TestStartTime = FPlatformTime::Seconds();
			const TSharedRef<bool> bHasStarted = MakeShared<bool>(false);
			const TSharedRef<FVector> StartLocation = MakeShared<FVector>(FVector::ZeroVector);
			const TSharedRef<double> MovementStartTime = MakeShared<double>(0.0);

			// O ticker roda uma vez por frame, permitindo que a simulacao continue entre chamadas.
			FTSTicker::GetCoreTicker().AddTicker(
				FTickerDelegate::CreateLambda([this, Done, TestStartTime, bHasStarted,
					StartLocation, MovementStartTime](float)
				{
					// Se o mapa ou o jogador ainda nao estiver pronto, aguardamos mais um frame.
					UWorld* PlayWorld = GEditor ? GEditor->PlayWorld : nullptr;
					APlayerController* PlayerController = PlayWorld
						? PlayWorld->GetFirstPlayerController()
						: nullptr;
					ACargoCharacter* Ship = PlayerController
						? Cast<ACargoCharacter>(PlayerController->GetPawn())
						: nullptr;

					if (!PlayWorld || !PlayWorld->HasBegunPlay() || !Ship)
					{
						// Evitamos deixar o teste esperando para sempre se a fase nao iniciar.
						if (FPlatformTime::Seconds() - TestStartTime > 20.0)
						{
							TestNotNull(TEXT("O mapa iniciou e o barco foi possuido pelo jogador"), Ship);
							if (GEditor && GEditor->PlayWorld)
							{
								GEditor->RequestEndPlayMap();
							}
							Done.ExecuteIfBound();
							return false;
						}
						return true;
					}

					// A funcao de movimento do barco espera o controller do jogo.
					if (!Cast<ACargoPlayerController>(PlayerController))
					{
						TestTrue(TEXT("O barco usa o controller do jogo Cargo"), false);
						GEditor->RequestEndPlayMap();
						Done.ExecuteIfBound();
						return false;
					}

					// Na primeira chamada guardamos a posicao inicial e iniciamos a contagem.
					if (!*bHasStarted)
					{
						*StartLocation = Ship->GetActorLocation();
						*MovementStartTime = PlayWorld->GetTimeSeconds();
						*bHasStarted = true;
					}

					// DoMove e a funcao publica de entrada de movimento usada pelos controles do barco.
					// Reabastecemos uma unidade por frame para o teste medir movimento, nao consumo de combustivel.
					if (ACargoPlayerState* PlayerState = PlayerController->GetPlayerState<ACargoPlayerState>())
					{
						PlayerState->GetFuelDomain()->AddFuel(1.0f);
					}
					Ship->DoMove(0.0f, 1.0f);

					// Esperamos cinco segundos de tempo simulado antes de verificar a posicao.
					if (PlayWorld->GetTimeSeconds() - *MovementStartTime < 5.0)
					{
						return true;
					}

					Ship->DoMove(0.0f, 0.0f);
					const float DistanceMoved = FVector::Dist(*StartLocation, Ship->GetActorLocation());
					TestTrue(TEXT("O barco mudou de posicao durante os cinco segundos"), DistanceMoved > 1.0f);

					// Encerramos o PIE para deixar o Editor pronto para o proximo teste.
					GEditor->RequestEndPlayMap();
					Done.ExecuteIfBound();
					return false;
				}));
		});
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS
