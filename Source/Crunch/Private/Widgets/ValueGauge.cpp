// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"

#include "DebugHelper.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();

	ProgressBar->SetFillColorAndOpacity(BarColor);
}

void UValueGauge::SetAndBoundToGameplayAttribute(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAttribute& Attribute,
	const FGameplayAttribute& MaxAttribute
)
{
	if (AbilitySystemComponent)
	{
		bool bFound;
		float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
		float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bFound);
		if (bFound)
		{
			SetValue(Value, MaxValue);
		}

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::ValueCharged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueGauge::MaxValueCharged);
	}
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	
	if (NewMaxValue == 0.f)
	{
		return;
	}

	const float NewPercent = NewValue / NewMaxValue;
	
	// ProgressBar의 퍼센트 값 갱신 (0.0 ~ 1.0 사이 값)
	ProgressBar->SetPercent(NewPercent);

	// 숫자 포맷팅 옵션: 소수점 없이 정수로 표시
	const FNumberFormattingOptions FormatOption = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	// ValueText를 "현재값/최대값" 형식으로 갱신
	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(NewValue, &FormatOption),
			FText::AsNumber(NewMaxValue, &FormatOption)
		)
	);
}

void UValueGauge::ValueCharged(const FOnAttributeChangeData& ChangedData)
{
	SetValue(ChangedData.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueCharged(const FOnAttributeChangeData& ChangedData)
{
	SetValue(CachedValue, ChangedData.NewValue);
}
