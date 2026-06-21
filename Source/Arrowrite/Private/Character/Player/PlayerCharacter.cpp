// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Player/PlayerCharacter.h"

#include "AbilitySystem/Data/CharacterStartupData.h"
#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "Animation/Player/PlayerAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/DeathmatchGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffectTypes.h"
#include "Input/InputConfigDataAsset.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/GamePlayerState.h"

APlayerCharacter::APlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	EquipmentComponent = CreateDefaultSubobject<UPlayerEquipmentComponent>(TEXT("EquipmentComponent"));
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, bBowAimPoseActive);
	DOREPLIFETIME(APlayerCharacter, bDeathStateActive);
	DOREPLIFETIME(APlayerCharacter, LastHitReactDirection);
}

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	if (!AbilitySystemComponent)
	{
		if (const AGamePlayerState* GamePlayerState = GetPlayerState<AGamePlayerState>())
		{
			return GamePlayerState->GetPlayerAbilitySystemComponent();
		}
	}

	return AbilitySystemComponent;
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EquipmentComponent)
	{
		EquipmentComponent->DestroyCarriedWeapons();
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::AddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority)
{
	if (!MappingContext)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		AddInputMappingContextLocal(MappingContext, Priority);
		return;
	}

	if (HasAuthority())
	{
		ClientAddInputMappingContext(MappingContext, Priority);
	}
}

void APlayerCharacter::RemoveInputMappingContext(UInputMappingContext* MappingContext)
{
	if (!MappingContext)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		RemoveInputMappingContextLocal(MappingContext);
		return;
	}

	if (HasAuthority())
	{
		ClientRemoveInputMappingContext(MappingContext);
	}
}

void APlayerCharacter::SetBowAimPoseActive(bool bShouldUseBowAimPose)
{
	if (bBowAimPoseActive == bShouldUseBowAimPose)
	{
		ApplyBowAimPoseActive();
		return;
	}

	bBowAimPoseActive = bShouldUseBowAimPose;
	ApplyBowAimPoseActive();

	if (!HasAuthority())
	{
		ServerSetBowAimPoseActive(bShouldUseBowAimPose);
	}
}

void APlayerCharacter::SetLastHitReactDirection(EHitReactDirection NewHitReactDirection)
{
	LastHitReactDirection = NewHitReactDirection;
}

void APlayerCharacter::EnterDeathState()
{
	SetDeathState(true);
}

void APlayerCharacter::ExitDeathState()
{
	SetDeathState(false);
}

void APlayerCharacter::RequestRespawn(float RespawnDelay)
{
	if (!HasAuthority())
	{
		ServerRequestRespawn(RespawnDelay);
		return;
	}

	if (ADeathmatchGameMode* DeathmatchGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADeathmatchGameMode>() : nullptr)
	{
		DeathmatchGameMode->RequestPlayerRespawn(GetController(), RespawnDelay);
	}
}

void APlayerCharacter::ApplyBowAimPoseActive()
{
	if (UPlayerAnimInstance* PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetMesh() ? GetMesh()->GetAnimInstance() : nullptr))
	{
		PlayerAnimInstance->SetBowAimPoseActive(bBowAimPoseActive);
	}
}

void APlayerCharacter::SetDeathState(bool bNewDeathState)
{
	if (!HasAuthority())
	{
		ServerSetDeathState(bNewDeathState);
		return;
	}

	if (bDeathStateActive == bNewDeathState)
	{
		ApplyDeathState();
		return;
	}

	bDeathStateActive = bNewDeathState;
	ApplyDeathState();

	if (bDeathStateActive)
	{
		RequestRespawn(RespawnDelayAfterDeath);
	}
}

void APlayerCharacter::ApplyDeathState()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();

	if (bDeathStateActive)
	{
		bIsSprinting = false;
		bBowAimPoseActive = false;
		ApplyBowAimPoseActive();

		if (MovementComponent)
		{
			MovementComponent->StopMovementImmediately();
			MovementComponent->DisableMovement();
		}

		if (CharacterCapsule)
		{
			CharacterCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		return;
	}

	if (CharacterCapsule)
	{
		CharacterCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (MovementComponent)
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
	}

	ApplyMovementSettings();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AddDefaultInputMappingContext();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	}

	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}

	if (RunAction)
	{
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ThisClass::StartSprinting);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ThisClass::StopSprinting);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Canceled, this, &ThisClass::StopSprinting);
	}

	if (AbilityInputConfig)
	{
		for (const FTaggedInputAction& TaggedInputAction : AbilityInputConfig->AbilityInputActions)
		{
			if (!TaggedInputAction.InputAction || !TaggedInputAction.InputTag.IsValid())
			{
				continue;
			}

			EnhancedInputComponent->BindAction(TaggedInputAction.InputAction, ETriggerEvent::Started, this, &ThisClass::AbilityInputTagPressed, TaggedInputAction.InputTag);
			EnhancedInputComponent->BindAction(TaggedInputAction.InputAction, ETriggerEvent::Completed, this, &ThisClass::AbilityInputTagReleased, TaggedInputAction.InputTag);
			EnhancedInputComponent->BindAction(TaggedInputAction.InputAction, ETriggerEvent::Canceled, this, &ThisClass::AbilityInputTagReleased, TaggedInputAction.InputTag);
		}
	}
}

void APlayerCharacter::AddInputMappingContextLocal(UInputMappingContext* MappingContext, int32 Priority) const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalController() || !MappingContext)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		InputSubsystem->AddMappingContext(MappingContext, Priority);
	}
}

void APlayerCharacter::RemoveInputMappingContextLocal(UInputMappingContext* MappingContext) const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalController() || !MappingContext)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		InputSubsystem->RemoveMappingContext(MappingContext);
	}
}

void APlayerCharacter::InitAbilityActorInfo()
{
	AGamePlayerState* GamePlayerState = GetPlayerState<AGamePlayerState>();
	if (!GamePlayerState)
	{
		return;
	}

	AbilitySystemComponent = GamePlayerState->GetPlayerAbilitySystemComponent();
	AttributeSet = GamePlayerState->GetPlayerAttributeSet();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(GamePlayerState, this);
		BindAttributeDelegates();
		GiveStartupAbilities();
	}
}

void APlayerCharacter::BindAttributeDelegates()
{
	if (bAttributeDelegatesBound || !AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedMultiplierAttribute()).AddUObject(this, &ThisClass::HandleMovementSpeedMultiplierChanged);
	SetMovementSpeedMultiplier(AttributeSet->GetMovementSpeedMultiplier());
	bAttributeDelegatesBound = true;
}

void APlayerCharacter::HandleMovementSpeedMultiplierChanged(const FOnAttributeChangeData& Data)
{
	SetMovementSpeedMultiplier(Data.NewValue);
}

void APlayerCharacter::GiveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent || !CharacterStartupData)
	{
		return;
	}

	CharacterStartupData->GiveToAbilitySystemComponent(AbilitySystemComponent);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bDeathStateActive)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller || MovementVector.IsNearlyZero())
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (!Controller || LookAxisVector.IsNearlyZero())
	{
		return;
	}

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::StartSprinting()
{
	if (bDeathStateActive)
	{
		return;
	}

	SetSprinting(true);
}

void APlayerCharacter::StopSprinting()
{
	SetSprinting(false);
}

void APlayerCharacter::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (bDeathStateActive)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void APlayerCharacter::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (bDeathStateActive)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void APlayerCharacter::AddDefaultInputMappingContext() const
{
	AddInputMappingContextLocal(DefaultMappingContext, 0);
}

void APlayerCharacter::ClientAddInputMappingContext_Implementation(UInputMappingContext* MappingContext, int32 Priority)
{
	AddInputMappingContextLocal(MappingContext, Priority);
}

void APlayerCharacter::ClientRemoveInputMappingContext_Implementation(UInputMappingContext* MappingContext)
{
	RemoveInputMappingContextLocal(MappingContext);
}

void APlayerCharacter::ServerSetBowAimPoseActive_Implementation(bool bShouldUseBowAimPose)
{
	SetBowAimPoseActive(bShouldUseBowAimPose);
}

void APlayerCharacter::ServerSetDeathState_Implementation(bool bNewDeathState)
{
	SetDeathState(bNewDeathState);
}

void APlayerCharacter::ServerRequestRespawn_Implementation(float RespawnDelay)
{
	RequestRespawn(RespawnDelay);
}

void APlayerCharacter::OnRep_BowAimPoseActive()
{
	ApplyBowAimPoseActive();
}

void APlayerCharacter::OnRep_DeathStateActive()
{
	ApplyDeathState();
}
