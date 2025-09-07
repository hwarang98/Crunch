// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	// 감지 대상 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;      // 적 감지 활성화
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;  // 아군 감지 비활성화
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;    // 중립 NPC 감지 비활성화

	// 시야 법위 설정
	SightConfig->SightRadius = InSightRadius;          // 기본 시야 반경
	SightConfig->LoseSightRadius = InLoseSightRadius;  // 목표를 놓치기 시작하는 최대 반경
	
	// 감지 데이터 유지 시간
	SightConfig->SetMaxAge(5.f); // 5초 동안 기억 (시간이 지나면 잊음)

	// 시야각 설정
	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 180도(전방 전체)를 시야각으로 설정

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::TargetForgotten);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	SetGenericTeamId(FGenericTeamId(0));

	if (IGenericTeamAgentInterface* PawnAsTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn))
	{
		PawnAsTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::TargetForgotten(AActor* TargetForgotten)
{
	if (!TargetForgotten)
	{
		return;
	}
	if (GetCurrentTarget() == TargetForgotten)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	return BlackboardComponent ? BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName) : nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	
	if (!BlackboardComponent)
	{
		return;
	}
	
	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
	}
	else
	{
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	if (const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget))
	{
		if (ActorASC->HasMatchingGameplayTag(CGameplayTags::State_Death))
		{
			// PerceptionComponent가 감지하고 있는 액터 목록을 순회
			for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator It = AIPerceptionComponent->GetPerceptualDataIterator(); It; ++It)
			{
				// 현재 순회 중인 액터가 ActorToForget이 아니라면 건너뜀
				if (It->Key != ActorToForget)
				{
					continue;
				}
				// ActorToForget이 맞다면 해당 액터의 감각 자극 정보(Stimuli)를 모두 확인
				for (FAIStimulus& Stimulus : It->Value.LastSensedStimuli)
				{
					// 자극의 나이를 float의 최대값으로 설정하여,
					// AI가 이 액터를 더 이상 인식하지 못하도록 "만료" 처리
					Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
				}
			}
		}
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		// 인식된 적대적 액터들을 담을 배열
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		// 이미 보고있는 경우
		if (Actors.Num() != 0)
		{
			return Actors[0];
		}
	}

	return nullptr;
}