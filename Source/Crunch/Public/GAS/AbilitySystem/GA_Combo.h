// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AbilitySystem/CGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Combo();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

private:
	void SetupWaitComboInputPress();
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;

	UPROPERTY(EditAnywhere, Category = "Attack | Trace Settings")
	float SphereSweepRadius = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Attack | Trace Settings")
	bool bDrawDebug = true;
	
	UPROPERTY(EditAnywhere, Category = "Attack | Trace Settings")
	bool bIgnoreSelf = true;

	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData GameplayEventData);

	UFUNCTION()
	void DoDamage(FGameplayEventData GameplayEventData);

	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	
	UFUNCTION()
	void TryCommitCombo();
	
	FName NextComboName;

	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;
};
