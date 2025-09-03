// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		FGameplayEffectSpec EffectSpec = *EffectSpecHandle.Data.Get();
		ApplyGameplayEffectSpecToSelf(EffectSpec);
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0,  static_cast<int32>(AbilityPair.Key), nullptr));
	}

	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& BaseAbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(BaseAbilityPair.Value, 1, static_cast<int32>(BaseAbilityPair.Key), nullptr));
	}
}
