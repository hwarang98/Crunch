// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/GAP_Launched.h"

#include "CGameplayTags.h"

UGAP_Launched::UGAP_Launched()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CGameplayTags::Ability_Passive_Launch_Activate;

	ActivationBlockedTags.RemoveTag(CGameplayTags::State_Stun);
	AbilityTriggers.Add(TriggerData);
}

void UGAP_Launched::ActivateAbility(
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

	if (K2_HasAuthority())
	{
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		K2_EndAbility();
	}
}
