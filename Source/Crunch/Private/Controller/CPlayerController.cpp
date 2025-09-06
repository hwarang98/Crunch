// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"
#include "Net/UnrealNetwork.h"


void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->ServerSideInit();
		PlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamID);
}

void ACPlayerController::SpawnGameplayWidget()
{
    // 현재 PlayerController가 로컬 컨트롤러인지 확인
    // (멀티플레이 환경에서는 서버와 클라이언트 모두 PlayerController를 가지지만,
    // UI는 오직 로컬 플레이어(내 화면)에만 띄워야 함)
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);

	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}
