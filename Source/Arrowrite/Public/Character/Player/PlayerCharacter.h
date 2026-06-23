// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BlueprintFunctionLibraries/CombatBlueprintLibrary.h"
#include "Character/BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "Interfaces/CombatUIInterface.h"
#include "PlayerCharacter.generated.h"

class UAbilitySystemComponent;
class UCameraComponent;
class UCharacterStartupData;
class UInputComponent;
class UInputConfigDataAsset;
class UInputAction;
class UInputMappingContext;
class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;
class UPlayerCombatUIComponent;
class UPlayerEquipmentComponent;
class USpringArmComponent;
struct FOnAttributeChangeData;
struct FInputActionValue;

UCLASS()
class ARROWRITE_API APlayerCharacter : public ABaseCharacter, public IAbilitySystemInterface, public ICombatUIInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "Ability System")
	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponent() const { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category = "Ability System")
	UPlayerAttributeSet* GetPlayerAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UPlayerEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	UFUNCTION(BlueprintCallable, Category = "Input")
	void AddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority = 1);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RemoveInputMappingContext(UInputMappingContext* MappingContext);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetBowAimPoseActive(bool bShouldUseBowAimPose);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetLastHitReactDirection(EHitReactDirection NewHitReactDirection);

	UFUNCTION(BlueprintPure, Category = "Combat")
	EHitReactDirection GetLastHitReactDirection() const { return LastHitReactDirection; }

	UFUNCTION(BlueprintCallable, Category = "Combat|Death")
	void EnterDeathState();

	UFUNCTION(BlueprintCallable, Category = "Combat|Death")
	void ExitDeathState();

	UFUNCTION(BlueprintPure, Category = "Combat|Death")
	bool IsDeathStateActive() const { return bDeathStateActive; }

	UFUNCTION(BlueprintCallable, Category = "Combat|Death")
	void RequestRespawn(float RespawnDelay = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "Combat|Damage")
	void ShowDamageNumber(float DamageAmount, FGameplayTag DamageTag, AActor* DamageSource);

	virtual UPlayerCombatUIComponent* GetPlayerCombatUIComponent_Implementation() const override { return PlayerCombatUIComponent; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void AddInputMappingContextLocal(UInputMappingContext* MappingContext, int32 Priority) const;
	void RemoveInputMappingContextLocal(UInputMappingContext* MappingContext) const;
	void ApplyBowAimPoseActive();
	void ApplyAimingGameplayTag();
	void SetDeathState(bool bNewDeathState);
	void ApplyDeathState();
	void InitAbilityActorInfo();
	void BindAttributeDelegates();
	void GiveStartupAbilities();
	void HandleMovementSpeedMultiplierChanged(const FOnAttributeChangeData& Data);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprinting();
	void StopSprinting();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AddDefaultInputMappingContext() const;

	UFUNCTION(Client, Reliable)
	void ClientAddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority);

	UFUNCTION(Client, Reliable)
	void ClientRemoveInputMappingContext(UInputMappingContext* MappingContext);

	UFUNCTION(Server, Reliable)
	void ServerSetBowAimPoseActive(bool bShouldUseBowAimPose);

	UFUNCTION(Server, Reliable)
	void ServerSetDeathState(bool bNewDeathState);

	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn(float RespawnDelay);

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastShowDamageNumber(float DamageAmount, FGameplayTag DamageTag, AActor* DamageSource);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Damage")
	void OnDamageNumberReceived(float DamageAmount, FGameplayTag DamageTag, AActor* DamageSource);

	UFUNCTION()
	void OnRep_BowAimPoseActive();

	UFUNCTION()
	void OnRep_DeathStateActive();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPlayerCombatUIComponent> PlayerCombatUIComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputConfigDataAsset> AbilityInputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterStartupData> CharacterStartupData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Death", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float RespawnDelayAfterDeath = 5.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_BowAimPoseActive, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bBowAimPoseActive = false;

	UPROPERTY(ReplicatedUsing = OnRep_DeathStateActive, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bDeathStateActive = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EHitReactDirection LastHitReactDirection = EHitReactDirection::Front;

	bool bAttributeDelegatesBound = false;

	FDelegateHandle MovementSpeedMultiplierChangedDelegateHandle;
};
