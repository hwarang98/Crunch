#pragma once

#include "Engine/Engine.h" // GEngine 사용을 위해 추가

namespace Debug
{
	// 기존 Print 함수들...

	// 아래 함수를 새로 추가
	static void Print(
		const FString& Category,
		const FString& Message,
		const FColor& Color = FColor::Yellow,
		int32 Inkey = -1)
	{
		if (GEngine)
		{
			const FString DebugMessage = FString::Printf(TEXT("[%s] %s"), *Category, *Message);
			GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, DebugMessage);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);
		}
	}

	static void Print(const FString& Message, const FColor& Color = FColor::MakeRandomColor(), int32 Inkey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, Message);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}

	static void Print(
		const FString& FloatTitle,
		float FloatValueToPrint,
		int32 key = -1,
		const FColor& Color = FColor::MakeRandomColor()
	)
	{
		if (GEngine)
		{
			const FString Message = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(key, 7.f, Color, Message);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}
}