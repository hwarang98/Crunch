// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/Attribute/CAttributeSet.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("Ability System Component");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("Attribute Set");
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyInitialEffects();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

