// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Spawners/MinionSpawner.h"

#include "Characters/MinionCharacter.h"
#include "GameFramework/PlayerStart.h"

AMinionSpawner::AMinionSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMinionSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &ThisClass::SpawnNewGroup, GroupSpawnInterval, true);
	}
}

void AMinionSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinionSpawner::SpawnNewMinions(const int Amount)
{
	for (int i = 0; i < Amount; ++i)
	{
		FTransform SpawnTransform = GetActorTransform();

		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		
		if (AMinionCharacter* NewMinion = GetWorld()->SpawnActorDeferred<AMinionCharacter>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
		{
			NewMinion->SetGenericTeamId(SpawnerTeamId);
			NewMinion->FinishSpawning(SpawnTransform);
			NewMinion->SetGoal(Goal);
			MinionPool.Add(NewMinion);
		}
	}
}

void AMinionSpawner::SpawnNewGroup()
{
	int i = MinionPerGroup;

	while (i > 0)
	{
		FTransform SpawnTransform = GetActorTransform();

		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		
		if (AMinionCharacter* NextAvailableMinion = GetNextAvailableMinion())
		{
			NextAvailableMinion->SetActorTransform(SpawnTransform);
			NextAvailableMinion->Active();
			--i;
		}
		else
		{
			break;
		}

	}
	
	SpawnNewMinions(i);
}

AMinionCharacter* AMinionSpawner::GetNextAvailableMinion() const
{
	for (AMinionCharacter* Minion : MinionPool)
	{
		if (!Minion->IsActive())
		{
			return Minion;
		}
	}
	return nullptr;
}

const APlayerStart* AMinionSpawner::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0)
	{
		return nullptr;
	}
	
	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

