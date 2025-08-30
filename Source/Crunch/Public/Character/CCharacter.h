// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CCharacter.generated.h"

class UCAttributeSet;
class UCAbilitySystemComponent;

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	void ServerSideInit();
	void ClientSideInit();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCAttributeSet* CAttributeSet;
};
