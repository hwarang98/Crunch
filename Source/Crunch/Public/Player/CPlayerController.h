// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UGameplayWidget;
class APawn;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 서버에서 Pawn을 컨트롤러에 소유(Possess)시킬 때 호출됨
	void OnPossess(APawn* NewPawn) override;
	
	// 클라이언트 측에서 서버가 지정한 Pawn을 최종적으로 확인(Acknowledge)할 때 호출됨, 리스닝 서버일 경우, 서버와 클라이언트 양쪽에서 모두 호출됨
	void AcknowledgePossession(APawn* NewPawn) override;

private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	ACPlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;
	
};
