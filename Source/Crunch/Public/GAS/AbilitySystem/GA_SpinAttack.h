// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AbilitySystem/CGameplayAbility.h"
#include "GAS/Types/CGameAbilityTypes.h"
#include "GA_SpinAttack.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_SpinAttack : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SpinAttack();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamageEffectDefinition> ComboDamageMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetSweepSphereRadius = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SpinningLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SpinningComboHoldSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SpinAttackMontage;

	UFUNCTION()
	void StartSpinning(FGameplayEventData EventData);
	
	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboCommitChangeEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);

	FName NextComboName;

	const FGenericDamageEffectDefinition* GetDamageEffectForCurrentCombo() const;
};
