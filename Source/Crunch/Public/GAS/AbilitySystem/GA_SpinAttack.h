// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AbilitySystem/CGameplayAbility.h"
#include "GA_SpinAttack.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_SpinAttack : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetSweepSphereRadius = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SpinningLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SpinAttackMontage;

	UFUNCTION()
	void StartSpinning(FGameplayEventData EventData);
};
