// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MinionCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void AMinionCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	PickSkinBasedOnTeamID();
}

bool AMinionCharacter::IsActive() const
{
	return !IsDead();
}

void AMinionCharacter::Active()
{
	RespawnImmediately();
}

void AMinionCharacter::SetGoal(AActor* GoalActor)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsObject(GoalBlackboardKeyName, GoalActor);
		}
	}
}

void AMinionCharacter::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}

/**
 * @brief 팀 ID에 기반하여 스켈레탈 메시(스킨)를 설정함으로써 캐릭터의 외형을 업데이트합니다.
 *
 * SkinMap은 FGenericTeamId와 USkeletalMesh 포인터 간의 매핑이며, 이 메서드는
 * 캐릭터의 외형이 소속된 팀에 맞게 설정되도록 보장합니다.
 *
 * 이 메서드는 캐릭터의 현재 팀 ID에 해당하는 스켈레탈 메시를 SkinMap에서 검색합니다.
 * 일치하는 스킨이 발견되면, 해당 스킨이 캐릭터의 메시에 적용됩니다.
 *
 * 
 * 1. 캐릭터의 현재 팀 ID를 사용하여 SkinMap에서 스켈레탈 메시 포인터를 검색합니다.
 * 2. 일치하는 스킨이 발견되면, 검색된 스켈레탈 메시를 사용하여 메시를 업데이트합니다.
 *
 */
void AMinionCharacter::PickSkinBasedOnTeamID()
{
	// 1. SkinMap에서 팀 ID에 해당하는 스킨을 찾음
	if (USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId()))
	{
		// 2. 스킨을 찾았다면, 캐릭터의 메시에 해당 스킨을 적용
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}
