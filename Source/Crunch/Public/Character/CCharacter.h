// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "CCharacter.generated.h"

struct FGameplayTag;
class UWidgetComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;

	// 서버에서만 호출
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/** 네트워크 복제에 사용되는 프로퍼티를 반환. 네이티브 복제 프로퍼티를 가진 모든 액터 클래스는 이 함수를 재정의 해야함. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Team
	/** 주어진 TeamID로 팀을 설정 */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** 팀 식별자를 FGenericTeamId 형태로 가져옴. */
	virtual FGenericTeamId GetGenericTeamId() const override;
#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
#pragma region Gameplay Ability System
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCAttributeSet* CAttributeSet;

	void BindGasChangeDelegates();
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
#pragma endregion

#pragma region UI
	
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	UWidgetComponent* OverHeadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityCheckUpdateGap = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityRangeSquared = 10000000.f;

	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;

	void UpdateHeadGaugeVisibility();
	void ConfigureOverHeadStatusWidget();
	void SetStatusGuageEnabled(bool bIsEnabled);
	
#pragma endregion

#pragma region Death And Respawn
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimerShift = -0.8f;

	void Respawn();
	void PlayDeathAnimation();
	void StartDeathSequence();
	void DeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnabled);

	virtual void OnDead();
	virtual void OnRespawn();

	FTimerHandle DeathMontageTimerHandle;
	FTransform MeshRelativeTransform;
	
#pragma endregion

#pragma region AI
	UPROPERTY();
	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;

	void SetAIPerceptionStimuliSourceEnable(bool bIsEnabled);
	
#pragma endregion

	
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
