// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ensureMsgf(MeshComp->GetOwner(), TEXT("MeshComp->GetOwner()가 nullptr입니다. 반드시 Owner가 있어야 합니다."));

	const UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());

	ensureMsgf(OwnerAbilitySystemComponent, TEXT("OwnerAbilitySystemComponent가 nullptr 입니다."));
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}

FString UAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		if (!TagNames.IsEmpty())
		{
			return TagNames.Last().ToString();
		}
	}

	return FString("None");
}