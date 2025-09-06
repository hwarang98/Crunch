// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/CGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

// 새로운 플레이어가 접속했을 때 호출되는 함수
APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    // 1. 부모 클래스의 함수를 호출하여 기본 플레이어 컨트롤러를 생성
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);

    // 2. 생성된 플레이어 컨트롤러에서 IGenericTeamAgentInterface를 가져옴
    // 이 인터페이스는 AI가 아군과 적군을 식별하는 데 사용
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);

    // 3. GetTeamIdForPlayer 함수를 호출하여 플레이어에게 할당할 팀 ID를 결정
    // (이 예제에서는 접속 순서에 따라 0번 팀과 1번 팀에 번갈아 배치.)
	FGenericTeamId TeamID = GetTeamIdForPlayer(NewPlayerController);

    // 4. 플레이어 컨트롤러가 팀 인터페이스를 지원하는 경우, 팀 ID를 설정
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}

    // 5. 할당된 팀에 맞는 시작 지점을 찾아 플레이어 컨트롤러의 StartSpot 속성에 설정
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamID);
	return NewPlayerController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(APlayerController* PlayerController) const
{
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	// 1. TeamID에 해당하는 시작 지점 태그(FName)를 TMap에서 찾음
	// TeamStartSpotTagName는 FGenericTeamId를 키로, FName을 값으로 가지는 맵
	const FName* StartSpotTag = TeamStartSpotTagName.Find(TeamID);
	
	if (!StartSpotTag)
	{
		return nullptr;
	}
	
	UWorld* World = GetWorld();

	// 2. 월드에 있는 모든 APlayerStart 액터를 순회
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		// 3. 현재 순회 중인 APlayerStart의 PlayerStartTag가 1번에서 찾은 태그와 일치하는지 확인합니다.
		if (It->PlayerStartTag == *StartSpotTag)
		{
			// 4. 일치하는 시작 지점을 찾았다면, 다른 플레이어가 이 지점을 다시 사용하지 못하도록 태그를 "Taken"으로 변경합니다.
			It->PlayerStartTag = FName("Taken");
			
			// 5. 찾은 APlayerStart 액터를 반환합니다.
			return *It;
		}
	}

	return nullptr;
}