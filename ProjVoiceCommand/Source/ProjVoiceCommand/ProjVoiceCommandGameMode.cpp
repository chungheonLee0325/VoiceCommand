// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjVoiceCommandGameMode.h"
#include "ProjVoiceCommandCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjVoiceCommandGameMode::AProjVoiceCommandGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
