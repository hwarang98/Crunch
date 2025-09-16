// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class APawn;
class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	/** 서버에서 Pawn을 컨트롤러에 소유(Possess)시킬 때 호출됨 */
	void OnPossess(APawn* NewPawn) override;
	
	/** 클라이언트 측에서 서버가 지정한 Pawn을 최종적으로 확인(Acknowledge)할 때 호출됨, 리스닝 서버일 경우, 서버와 클라이언트 양쪽에서 모두 호출됨 */
	void AcknowledgePossession(APawn* NewPawn) override;

	/** 주어진 TeamID로 팀을 설정 */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** 팀 식별자를 FGenericTeamId 형태로 가져옴. */
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** 네트워크 복제에 사용되는 프로퍼티를 반환. 네이티브 복제 프로퍼티를 가진 모든 액터 클래스는 이 함수를 재정의 해야함. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	ACPlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
