// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CGameAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None             UMETA(DisplayName="None"), 
	BasicAttack      UMETA(DisplayName="Basic Attack"),
	AbilityOne       UMETA(DisplayName="Ability One"),
	AbilityTwo       UMETA(DisplayName="Ability Two"),
	AbilityThree     UMETA(DisplayName="Ability Three"),
	AbilityFour      UMETA(DisplayName="Ability Four"),
	AbilityFive      UMETA(DisplayName="Ability Five"),
	Confirm          UMETA(DisplayName="Confirm"),
	Cancel           UMETA(DisplayName="Cancel"),
};

USTRUCT(BlueprintType)
struct FGenericDamageEffectDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
};
