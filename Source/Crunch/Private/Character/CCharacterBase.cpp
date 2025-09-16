// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/CCharacterBase.h"

#include "CGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/Attribute/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverHeadStatsGauge.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

#include "DebugHelper.h"

// const FObjectInitializer& ObjectInitializer
// : Super(ObjectInitializer)
ACCharacterBase::ACCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 컴포넌트 부착
	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("Ability System Component");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("Attribute Set");
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Over Head Widget Component");

	// 초기값 세팅
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGasChangeDelegates();

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");
}

void ACCharacterBase::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyInitialEffects();
	AbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacterBase::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacterBase::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

bool ACCharacterBase::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(CGameplayTags::State_Death);
}

void ACCharacterBase::RespawnImmediately()
{
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(CGameplayTags::State_Death));
	}
}

void ACCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 해당경우 AI Controller
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatusWidget();
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACCharacterBase::BindGasChangeDelegates()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::State_Death).AddUObject(this, &ACCharacterBase::DeathTagUpdated);
	}
}

void ACCharacterBase::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacterBase::ConfigureOverHeadStatusWidget()
{
	check(OverHeadWidgetComponent);

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());

	check(OverHeadStatsGauge)

	OverHeadStatsGauge->ConfigureWithAbilitySystemComponent(GetAbilitySystemComponent());
	OverHeadWidgetComponent->SetHiddenInGame(false);
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this, &ThisClass::UpdateHeadGaugeVisibility, HeadStatGaugeVisibilityCheckUpdateGap, true);
}

void ACCharacterBase::SetStatusGuageEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		ConfigureOverHeadStatusWidget();
	}
	else
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

void ACCharacterBase::UpdateHeadGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

// Called every frame
void ACCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacterBase, TeamID);
}

void ACCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

void ACCharacterBase::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &ThisClass::DeathMontageFinished, MontageDuration + DeathMontageFinishTimerShift);
	}
}

void ACCharacterBase::StartDeathSequence()
{
	OnDead();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();
	}
	
	PlayDeathAnimation();
	SetStatusGuageEnabled(false);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnable(false);
}

void ACCharacterBase::DeathMontageFinished()
{
	if (IsDead())
	{
		SetRagdollEnabled(true);
	}
}

void ACCharacterBase::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		// GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacterBase::Respawn()
{
	OnRespawn();
	SetAIPerceptionStimuliSourceEnable(true);
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGuageEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ApplyFullStatEffect();
	}
}

void ACCharacterBase::OnDead()
{
}

void ACCharacterBase::OnRespawn()
{
}

void ACCharacterBase::SetAIPerceptionStimuliSourceEnable(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent)
	{
		return;
	}

	if (bIsEnabled)
	{
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem(); // RegisterWithPerceptionSystem 등록
	}
	else
	{
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem(); // RegisterWithPerceptionSystem 해제
	}
}

void ACCharacterBase::OnRep_TeamID()
{
}

