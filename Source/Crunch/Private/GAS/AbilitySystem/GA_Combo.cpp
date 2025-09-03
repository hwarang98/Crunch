// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/GA_Combo.h"
#include "Gas/Statics/CAbilitySystemStatics.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "DebugHelper.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(
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

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboChangedEventTag(), nullptr, false, false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &ThisClass::ComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboTargetEventTag());
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &ThisClass::DoDamage);
		WaitTargetingEventTask->ReadyForActivation();
	}
	SetupWaitComboInputPress();
	

}

FGameplayTag UGA_Combo::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change");
}

FGameplayTag UGA_Combo::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change.End");
}

FGameplayTag UGA_Combo::GetComboTargetEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Damage");
}

void UGA_Combo::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData GameplayEventData)
{
	FGameplayTag EventTag = GameplayEventData.EventTag;

	if (EventTag == GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		Debug::Print(TEXT("Next Combo is cleared"));
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();

	Debug::Print(TEXT("Next Combo Is Now: "), *NextComboName.ToString());
}

void UGA_Combo::DoDamage(FGameplayEventData GameplayEventData)
{
	TArray<FHitResult> HitResult = GetHitResultFromSweepLocationTargetData(GameplayEventData.TargetData, SphereSweepRadius, bDrawDebug, bIgnoreSelf);
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (!OwnerAnimInstance)
	{
		return;
	}

	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
}

