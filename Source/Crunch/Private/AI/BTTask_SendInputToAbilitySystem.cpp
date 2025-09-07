// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SendInputToAbilitySystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

/**
 * 특정 게임플레이 어빌리티를 발동시키기 위해 어빌리티 시스템 컴포넌트에 입력 신호를 전달하는 태스크입니다.
 * AI 컨트롤러가 조종하는 폰(Pawn)에서 어빌리티 시스템 컴포넌트를 가져온 후, 해당 컴포넌트에 설정된 입력 ID를 활성화합니다.
 *
 * @param OwnerComp 실행 중인 행동 트리의 UBehaviorTreeComponent 참조입니다.
 * @param NodeMemory BTTask 노드 인스턴스의 메모리에 대한 포인터입니다.
 * @return 태스크 실행 결과를 나타내는 EBTNodeResult::Type 값입니다. 성공적으로 실행되면 EBTNodeResult::Succeeded, 실패 시 EBTNodeResult::Failed를 반환합니다.
 */
EBTNodeResult::Type UBTTask_SendInputToAbilitySystem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const AAIController* OwnerAIController = OwnerComp.GetAIOwner())
	{
		// AI 컨트롤러가 조종하는 폰(Pawn)으로부터 어빌리티 시스템 컴포넌트(AbilitySystemComponent)를 가져옴
		if (UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerAIController->GetPawn()))
		{
			// 어빌리티 시스템 컴포넌트에 설정된 InputID를 눌렀다는 신호를 보냄
			// 이는 특정 게임플레이 어빌리티를 발동시키는 트리거 역할을 함
			OwnerAbilitySystemComponent->PressInputID(static_cast<int32>(InputID));
			
			// 태스크가 성공적으로 실행되었음을 행동 트리에 알림
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
