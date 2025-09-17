// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/GA_SpinAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "DebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UGA_SpinAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	// 서버 권한(authority) 또는 클라이언트의 Prediction Key 가 유효한지를 체크하는 함수
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySpinAttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None, SpinAttackMontage);
		PlaySpinAttackMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);    // 몽타주가 블렌드 아웃될 때 호출
		PlaySpinAttackMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);   // 몽타주가 취소될 때 호출
		PlaySpinAttackMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility); // 몽타주가 인터럽트될 때 호출
		PlaySpinAttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);   // 몽타주가 정상적으로 끝났을 때 호출
		PlaySpinAttackMontageTask->ReadyForActivation(); // Task를 실제로 실행 시작

		UAbilityTask_WaitGameplayEvent* WaitSpinningEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CGameplayTags::Ability_SpinAttack);
		WaitSpinningEventTask->EventReceived.AddDynamic(this, &ThisClass::StartSpinning);
		WaitSpinningEventTask->ReadyForActivation();
	}
}

void UGA_SpinAttack::StartSpinning(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug(), true);
		
		for (FHitResult& HitResult : TargetHitResults)
		{
			if (AActor* TargetActor = HitResult.GetActor())
			{
				Debug::Print(TEXT("Spin Attack Hit: "), TargetActor->GetName());
				// 플레이어 위치에서 타겟 위치로의 방향 벡터를 계산
				FVector PushDirection = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();
				
				// 위쪽 방향을 살짝 추가하여 자연스럽게 뜨면서 밀려남
				FVector LaunchVelocity = (PushDirection + FVector::UpVector * 0.5f).GetSafeNormal() * SpinningLaunchSpeed;
				PushTarget(TargetActor, LaunchVelocity);
				ApplyGameplayEffectToHitResultActor(HitResult, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			}
		}
	}
}
