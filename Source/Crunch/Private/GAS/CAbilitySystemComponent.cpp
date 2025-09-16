// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"
#include "GAS/Attribute/CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthUpdated);
}

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

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStateEffect);
}

/**
 * AuthApplyGameplayEffect는 지정된 GameplayEffect를 특정 레벨로 적용합니다.
 * 이 메서드는 권한을 가진(Aggregate Authority) 소유자에서만 호출될 수 있습니다.
 *
 * @param GameplayEffect 적용할 UGameplayEffect의 클래스입니다.
 * @param Level GameplayEffect를 적용할 레벨 값입니다.
 */
void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		FGameplayEffectSpec EffectSpec = *EffectSpecHandle.Data.Get();
		ApplyGameplayEffectSpecToSelf(EffectSpec);
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner())
	{
		return;
	}

	if (ChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}
