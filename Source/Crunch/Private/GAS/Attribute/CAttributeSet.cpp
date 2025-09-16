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

/**
 * 속성 변경 전에 호출되는 함수
 *
 * 이 함수는 Health 및 Mana 속성이 변경되기 전에 실행되며,
 * 새 값(NewValue)이 유효한 범위 내에 있도록 보장하기 위해 값을 클램핑합니다.
 * Health 속성은 0과 MaxHealth 사이의 값으로 제한되며,
 * Mana 속성은 0과 MaxMana 사이의 값으로 제한됩니다.
 *
 * @param Attribute 변경될 속성의 참조입니다. Health 또는 Mana와 관련된 속성을 전달받을 수 있습니다.
 * @param NewValue 변경될 속성의 새 값입니다. 이 값이 적절한 범위 내로 클램핑됩니다.
 */
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

/**
 * GameplayEffect 실행 후 속성 값을 처리
 *
 * 이 함수는 GameplayEffect 실행 후 속성 값이 변경된 경우 호출되며,
 * 변경된 속성 값을 특정 범위로 제한하거나 추가 로직을 실행하는 데 사용.
 * Health 및 Mana 속성 값에 대해 제한된 값 범위를 적용해 주어진 Max 값을 초과하거나 0보다 작아지지 않도록 함.
 *
 * @param Data 변경된 속성 값과 관련 메타 데이터를 포함한 FGameplayEffectModCallbackData 구조체.
 */
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
