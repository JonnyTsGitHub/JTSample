// Copyright Epic Games, Inc. All Rights Reserved.

#include "JTSampleGameMode.h"
#include "JTSampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AJTSampleGameMode::AJTSampleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
