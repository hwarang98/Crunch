// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CCharacterBase.h"
#include "MinionCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API AMinionCharacter : public ACCharacterBase
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	bool IsActive() const;
	void Active();
	void SetGoal(AActor* GoalActor);

private:
	virtual void OnRep_TeamID() override;
	
	void PickSkinBasedOnTeamID();

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName GoalBlackboardKeyName = FName("Goal");

	
};
