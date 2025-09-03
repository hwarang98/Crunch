// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || TargetSocketNames.Num() <= 1 || !MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return;
	}
	FGameplayEventData EventData;

	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		// heap allocation
		FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();

		FVector StartLocation = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
		FVector EndLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLocation);
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLocation);

		EventData.TargetData.Add(LocationInfo);
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
	
}
