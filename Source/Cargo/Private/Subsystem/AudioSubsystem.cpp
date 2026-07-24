// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/AudioSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	BGMAudioComp = NewObject<UAudioComponent>(this);
	BGMAudioComp->bAutoActivate = false;
	BGMAudioComp->bAutoDestroy = false;
	BGMAudioComp->RegisterComponentWithWorld(GetWorld());
}

void UAudioSubsystem::PlayBGM(USoundBase* BGM)
{
	BGMAudioComp->FadeOut(FadeDuration, 0.0f);

	BGMAudioComp = UGameplayStatics::SpawnSound2D(this, BGM, 1.0f, 1.0f, 0.0f, nullptr, true, true);
	BGMAudioComp->FadeIn(FadeDuration, 1.0f);
}

void UAudioSubsystem::StopBGM()
{
	BGMAudioComp->FadeOut(FadeDuration, 0.0f);
}

void UAudioSubsystem::SetBGMVolume(float NewVolume)
{
	BGMAudioComp->AdjustVolume(FadeDuration, NewVolume);
}
