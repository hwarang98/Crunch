// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/CPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemComponent.h"

// ACPlayerCharacter::ACPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
ACPlayerCharacter::ACPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubSystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubSystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubSystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLoopInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

		for (const TPair<EAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &ThisClass::HandleAbilityInput, InputActionPair.Key);
		}
	}
}

void ACPlayerCharacter::OnAttackHitBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority() || !OtherActor)
	{
		return;
	}

	// 자기 자신은 무시
	if (OtherActor == this)
	{
		return;
	}
	
	if (IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(this))
	{
		// OtherActor에 대한 이 캐릭터의 팀 관계를 확인
		ETeamAttitude::Type Attitude = OwnerTeamInterface->GetTeamAttitudeTowards(*OtherActor);
       
		// 적대적(Hostile) 관계가 아니라면 무시
		if (Attitude != ETeamAttitude::Hostile)
		{
			return;
		}
	}
}

void ACPlayerCharacter::SetInputEnableFromPlayerController(const bool bEnable)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}
	
	if (bEnable)
	{
		EnableInput(PlayerController);
	}
	else
	{
		DisableInput(PlayerController);
	}
}

void ACPlayerCharacter::OnDead()
{
	SetInputEnableFromPlayerController(bDisableInput);
}

void ACPlayerCharacter::OnRespawn()
{
	SetInputEnableFromPlayerController(bEnableInput);
}

void ACPlayerCharacter::OnStun()
{
	SetInputEnableFromPlayerController(bDisableInput);
}

void ACPlayerCharacter::OnRecoverFromStun()
{
	if (IsDead())
	{
		return;
	}
	
	SetInputEnableFromPlayerController(bEnableInput);
}

void ACPlayerCharacter::HandleLoopInput(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = GetInputActionValue(InputActionValue);

	AddControllerPitchInput(-InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputValue = GetInputActionValue(InputActionValue);
	InputValue.Normalize();

	AddMovementInput(GetMoveForwardDirection() * InputValue.Y + GetLookRightDirection() * InputValue.X);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, EAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed(static_cast<int32>(InputID));
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased(static_cast<int32>(InputID));
	}
	
	if (InputID == EAbilityInputID::BasicAttack)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, CGameplayTags::Ability_BasicAttack_Pressed, FGameplayEventData());
		Server_SendGameplayEventToSelf(CGameplayTags::Ability_BasicAttack_Pressed, FGameplayEventData());
	}
}

FVector2D ACPlayerCharacter::GetInputActionValue(const FInputActionValue& InputActionValue)
{
	return InputActionValue.Get<FVector2D>();
}

FVector ACPlayerCharacter::GetLookRightDirection() const
{
	return ViewCamera->GetRightVector();
}

FVector ACPlayerCharacter::GetLookForwardDirection() const
{
	return ViewCamera->GetForwardVector();
}

FVector ACPlayerCharacter::GetMoveForwardDirection() const
{
	return FVector::CrossProduct(GetLookRightDirection(), FVector::UpVector);
}

