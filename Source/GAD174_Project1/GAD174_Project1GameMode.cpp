// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAD174_Project1GameMode.h"
#include "GAD174_Project1Character.h"
#include "UObject/ConstructorHelpers.h"

AGAD174_Project1GameMode::AGAD174_Project1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
