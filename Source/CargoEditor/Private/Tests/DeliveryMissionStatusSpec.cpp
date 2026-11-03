// Testes de automacao so sao incluidos em builds de desenvolvimento.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Mission/DeliveryMissionData.h"
#include "Mission/DeliveryMissionStatus.h"

// BEGIN_DEFINE_SPEC declara um teste organizado em cenarios (Describe/It).
BEGIN_DEFINE_SPEC(FDeliveryMissionStatusSpec,
	"Cargo.Mission.DeliveryMissionStatus",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FDeliveryMissionStatusSpec)

void FDeliveryMissionStatusSpec::Define()
{
	// Describe agrupa os cenarios relacionados a uma funcionalidade.
	Describe("Progresso da carga entregue", [this]()
	{
		// It descreve um comportamento que esperamos do codigo.
		It("aumenta em uma unidade ao registrar uma entrega", [this]()
		{
			// Criamos dados de missao temporarios, sem carregar um mapa ou asset.
			UDeliveryMissionData* MissionData = NewObject<UDeliveryMissionData>();

			// Usamos uma tag de carga que ja existe nas configuracoes do projeto.
			const FGameplayTag CoffeeTag = FGameplayTag::RequestGameplayTag(
				FName("Cargo.Coffee"));

			// Dizemos que a missao exige 3 unidades de cafe.
			FCargoRequirement Requirement;
			Requirement.CargoType = CoffeeTag;
			Requirement.Quantity = 3;
			MissionData->CargoRequirements.Add(Requirement);

			// Criamos e inicializamos o objeto de status que queremos testar.
			UDeliveryMissionStatus* MissionStatus = NewObject<UDeliveryMissionStatus>();
			MissionStatus->Initialize(MissionData, FGameplayTag());

			// Simulamos a entrega de uma unidade, chamando diretamente o codigo C++.
			MissionStatus->AddCargoDelivery(CoffeeTag);

			// Lemos o progresso e comparamos com o valor esperado: 1.
			const TMap<FGameplayTag, FCargoStatus> DeliveredQuantities =
				MissionStatus->GetDeliveredQuantities();
			const FCargoStatus* CoffeeStatus = DeliveredQuantities.Find(CoffeeTag);

			TestNotNull(TEXT("A carga de cafe existe no progresso da missao"), CoffeeStatus);
			if (CoffeeStatus)
			{
				TestEqual(TEXT("Uma entrega registra uma unidade"),
					CoffeeStatus->DeliveredQuantity, 1);
			}
		});
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS
