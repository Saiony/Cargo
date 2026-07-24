// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TObjectPtr<UAudioComponent> BGMAudioComp;

	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float FadeDuration = 3.0f;	
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;	
	
	static UAudioSubsystem* Get(const UObject* WorldContextObject) { return WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UAudioSubsystem>(); }
	
	UFUNCTION(BlueprintCallable, Category="Cargo")
	void PlayBGM(USoundBase* BGM);
	
	void StopBGM();
	void SetBGMVolume(float NewVolume);
};
