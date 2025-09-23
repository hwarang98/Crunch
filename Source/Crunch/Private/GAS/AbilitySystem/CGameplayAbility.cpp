// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystem/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "GameFramework/Character.h"
#include "GAS/AbilitySystem/GAP_Launched.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
	ActivationBlockedTags.AddTag(CGameplayTags::State_Stun);
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (SkeletalMeshComponent)
	{
		return SkeletalMeshComponent->GetAnimInstance();
	}

	return nullptr;
}

/**
 * @brief 타겟 데이터 핸들에서 스윕 위치를 기반으로 피격 결과를 가져
 * @param TargetDataHandle 스윕의 시작 및 끝 위치를 포함하는 타겟 데이터
 * @param SphereSweepRadius 스윕에 사용할 구의 반지름
 * @param TargetTeam 필터링할 대상의 팀 관계입니다. (예: 적, 아군)
 * @param bDrawDebug 디버그용 스윕 라인을 그릴지 여부
 * @param bIgnoreSelf 어빌리티 시전자 자신을 무시할지 여부
 * @return 조건에 맞는 필터링된 피격 결과 배열을 반환
 */
TArray<FHitResult> UCGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	float SphereSweepRadius,
	ETeamAttitude::Type TargetTeam,
	bool bDrawDebug,
	bool bIgnoreSelf
) const
{
	// 최종적으로 반환할 유효한 피격 결과들을 저장할 배열
	TArray<FHitResult> OutResult;
	// 이미 처리한 액터를 추적하여 중복 처리를 방지하기 위한 집합
	TSet<AActor*> HitActors;

	// 입력받은 모든 타겟 데이터를 순회
	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		// 스윕의 시작 위치와 끝 위치를 타겟 데이터에서 가져옴
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();

		// 스윕이 감지할 오브젝트 타입을 설정합니다. 여기서는 Pawn만 감지하도록 함
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		// 스윕에서 무시할 액터 목록
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			// bIgnoreSelf가 true이면, 어빌리티 시전자 자신을 무시 목록에 추가
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		// bDrawDebug 값에 따라 디버그 드로잉 옵션을 설정
		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		// 스윕 결과를 저장할 배열
		TArray<FHitResult> HitResults;
		
		// 구체 스윕을 수행하여 지정된 오브젝트 타입과 충돌하는 모든 액터를 찾음
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StartLocation,
			EndLocation,
			SphereSweepRadius,
			ObjectTypes,
			false, // bTraceComplex
			ActorsToIgnore,
			DrawDebugTrace,
			HitResults,
			bIgnoreSelf
		);

		// 스윕으로 감지된 모든 피격 결과를 순회하며 필터링
		for (const FHitResult& HitResult : HitResults)
		{
			// 이미 처리된 액터인 경우 건너뜁 (중복 방지).
			if (HitActors.Contains(HitResult.GetActor()))
			{
				continue;
			}

			// 어빌리티 시전자의 팀 인터페이스를 가져옴
			if (IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo()))
			{
				// 피격된 액터에 대한 시전자의 팀 관계를 확인
				ETeamAttitude::Type OtherActorTeamAttribute = OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor());
				// 원하는 팀 관계(TargetTeam)가 아니면 건너뜁
				if (OtherActorTeamAttribute != TargetTeam)
				{
					continue;
				}
			}
			
			// 유효한 피격이므로, 액터를 처리된 목록에 추가하고 결과를 최종 결과 배열에 추가
			HitActors.Add(HitResult.GetActor());
			OutResult.Add(HitResult);
		}
	}
	// 필터링된 최종 피격 결과 배열을 반환
	return OutResult;
}

void UCGameplayAbility::PushSelf(const FVector& PushVelocity)
{
	if (ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter())
	{
		OwningAvatarCharacter->LaunchCharacter(PushVelocity, true, true);
	}
}

/**
 * @brief 지정된 타겟을 지정된 속도로 밀어냅니다.
 * @param Target 밀어낼 대상 액터
 * @param PushVelocity 밀어내는 데 사용할 속도 및 방향
 *
 * 주어진 타켓에 대해 게임플레이 이벤트 데이터를 생성하고 이를 활용하여 밀어내는 처리를 수행합니다.
 */
void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVelocity)
{
	if (!Target)
	{
		return;
	}

	// GAS에서 액터에게 이벤트를 보낼 때 쓰는 구조체 -> Push 효과를 이벤트로 전달
	FGameplayEventData EventData;

	// 단일 타겟 HIT 데이터 생성
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit;

	// 히트 결과 구조체 준비
	FHitResult HitResult;
	
	// ImpactPoint에 Push 방향과 세기 정보를 임시로 담음
	// (실제 위치가 아니라 힘 벡터를 이벤트로 전달하는 용도)
	HitResult.ImpactNormal= PushVelocity;
	
	// HitData에 HitResult 연결
	HitData->HitResult = HitResult;
	
	// 이벤트 데이터에 HitData 추가
	EventData.TargetData.Add(HitData);

	// Target에게 지정된 태그 이벤트를 보냄
	// 이벤트 수신자는 이 데이터를 받아 실제 밀림 효과를 처리
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Target, // 이벤트를 받을 액터
		CGameplayTags::Ability_Passive_Launch_Activate,  // 이벤트 태그
		EventData // 이벤트 데이터
	);
}

void UCGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContextHandle.AddHitResult(HitResult);

	EffectSpecHandle.Data->SetContext(EffectContextHandle);

	ApplyGameplayEffectSpecToTarget(
		GetCurrentAbilitySpecHandle(),
		CurrentActorInfo,
		CurrentActivationInfo,
		EffectSpecHandle,
		UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor())
	);
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}

	return AvatarCharacter;
}
