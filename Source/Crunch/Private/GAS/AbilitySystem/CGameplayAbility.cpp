// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/CGameplayAbility.h"

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (SkeletalMeshComponent)
	{
		return SkeletalMeshComponent->GetAnimInstance();
	}

	return nullptr;
}
