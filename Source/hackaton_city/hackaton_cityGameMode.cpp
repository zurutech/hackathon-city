// Copyright Epic Games, Inc. All Rights Reserved.

#include "hackaton_cityGameMode.h"
#include "hackaton_cityCharacter.h"
#include "UObject/ConstructorHelpers.h"

Ahackaton_cityGameMode::Ahackaton_cityGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
