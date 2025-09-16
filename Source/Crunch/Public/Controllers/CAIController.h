// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

struct FGameplayTag;
struct FAIStimulus;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

	virtual void OnPossess(APawn* NewPawn) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName = FName("Target");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "Perception | Sight")
	float InSightRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Perception | Sight")
	float InLoseSightRadius = 1200.f;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

	UFUNCTION()
	void TargetForgotten(AActor* TargetForgotten);

	const UObject* GetCurrentTarget() const;
	AActor* GetNextPerceivedActor() const;
	void SetCurrentTarget(AActor* NewTarget);
	void ForgetActorIfDead(AActor* ActorToForget);
	void ClearAndDisableAllSenses();
	void EnableAllSenses();
	void PawnDeadTagUpdated(const FGameplayTag InTag, int32 Count);

	const float NumberUnLimit = TNumericLimits<float>::Max();
};

