// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "MinionSpawner.generated.h"

class AMinionCharacter;

UCLASS()
class CRUNCH_API AMinionSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinionSpawner();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId SpawnerTeamId;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinionCharacter> MinionClass;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart*> SpawnSpots;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int MinionPerGroup = 3;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GroupSpawnInterval = 5.f;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	AActor* Goal;

	UPROPERTY()
	TArray<AMinionCharacter*> MinionPool;

	void SpawnNewMinions(const int Amount);
	void SpawnNewGroup();

	AMinionCharacter* GetNextAvailableMinion() const;

	int NextSpawnSpotIndex = -1;

	const APlayerStart* GetNextSpawnSpot();

	FTimerHandle SpawnIntervalTimerHandle;
};
