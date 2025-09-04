// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attribute/CAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

/**
 * Health 속성의 리플리케이션을 처리
 *
 * 이 함수는 리플리케이트된 인스턴스에서 Health 속성이 갱신될 때 호출.
 * 네트워크 환경에서 Health 속성에 가해진 변경 사항이 올바르게 전파되도록 보장하며, 속성이 변경될 때 추가적인 로직을 실행할 수 있도록 함.
 *
 * @param OldValue 업데이트되기 전 Health 속성의 이전 값.
 */
void UCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, Health, OldValue); // 체력 속성이 복제될 때 호출
}

/**
 * MaxHealth 속성의 리플리케이션을 처리
 *
 * 이 함수는 리플리케이트된 인스턴스에서 Health 속성이 갱신될 때 호출.
 * 네트워크 환경에서 Health 속성에 가해진 변경 사항이 올바르게 전파되도록 보장하며, 속성이 변경될 때 추가적인 로직을 실행할 수 있도록 함.
 *
 * @param OldValue 업데이트되기 전 Health 속성의 이전 값.
 */
void UCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MaxHealth, OldValue); // 최대체력 속성이 복제될 때 호출
}

/**
 * Mana 속성의 리플리케이션을 처리
 *
 * 이 함수는 리플리케이트된 인스턴스에서 Health 속성이 갱신될 때 호출.
 * 네트워크 환경에서 Health 속성에 가해진 변경 사항이 올바르게 전파되도록 보장하며, 속성이 변경될 때 추가적인 로직을 실행할 수 있도록 함.
 *
 * @param OldValue 업데이트되기 전 Health 속성의 이전 값.
 */
void UCAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, Mana, OldValue); // 최대체력 속성이 복제될 때 호출
}

/**
 * MaxMana 속성의 리플리케이션을 처리
 *
 * 이 함수는 리플리케이트된 인스턴스에서 Health 속성이 갱신될 때 호출.
 * 네트워크 환경에서 Health 속성에 가해진 변경 사항이 올바르게 전파되도록 보장하며, 속성이 변경될 때 추가적인 로직을 실행할 수 있도록 함.
 *
 * @param OldValue 업데이트되기 전 Health 속성의 이전 값.
 */
void UCAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MaxMana, OldValue); // 최대체력 속성이 복제될 때 호출
}

/**
 * 오브젝트 속성의 리플리케이션 설정을 처리
 *
 * 이 함수는 네트워크 환경에서 속성들의 리플리케이션 동작을 정의하기 위해 호출됨.
 * 지정된 속성들이 네트워크를 통해 올바르게 전파되도록 보장하며, 조건과 알림 설정을 설정할 수 있음.
 *
 * @param OutLifetimeProps 리플리케이션 동작이 적용될 속성을 저장하는 배열.
 */
void UCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Health, COND_None, REPNOTIFY_Always) // 어떤 프로퍼티를 네트워크로 동기화할지 등록하는 함수
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Mana, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxMana, COND_None, REPNOTIFY_Always)
}

void UCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}
}
