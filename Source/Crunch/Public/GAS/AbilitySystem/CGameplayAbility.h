// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(
		const FGameplayAbilityTargetDataHandle& TargetDataHandle,
		float SphereSweepRadius = 30.f,
		ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile,
		bool bDrawDebug = false,
		bool bIgnoreSelf = true
	) const;

	void PushSelf(const FVector& PushVelocity);
	void PushTarget(AActor* Target, const FVector& PushVelocity);
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	ACharacter* GetOwningAvatarCharacter();

	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDrawDebug = false;

	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
};
