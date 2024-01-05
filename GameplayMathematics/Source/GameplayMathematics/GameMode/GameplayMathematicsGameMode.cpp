// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayMathematicsGameMode.h"
#include "../Character/GameplayMathematicsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGameplayMathematicsGameMode::AGameplayMathematicsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
