// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTagContainer.h"
#include "GAS/Statics/CAbilitySystemStatics.h"

FGameplayTag UCAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}
