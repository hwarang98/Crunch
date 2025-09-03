// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/CGameplayAbility.h"

#include "Kismet/KismetSystemLibrary.h"

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (SkeletalMeshComponent)
	{
		return SkeletalMeshComponent->GetAnimInstance();
	}

	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	float SphereSweepRadius,
	bool bDrawDebug,
	bool bIgnoreSelf
) const
{
	TArray<FHitResult> OutResult;

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> HitResults;
		
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StartLocation,
			EndLocation,
			SphereSweepRadius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			DrawDebugTrace,
			HitResults,
			false
		);
		
	}
	return OutResult;
}
