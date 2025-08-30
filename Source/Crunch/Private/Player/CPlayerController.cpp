// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"

#include "Player/CPlayerCharacter.h"


void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->ServerSideInit();
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->ClientSideInit();
	}
}
