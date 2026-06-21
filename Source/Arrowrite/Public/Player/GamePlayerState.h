// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerScoreChangedDelegate, int32, NewValue, int32, OldValue);

UCLASS()
class ARROWRITE_API AGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGamePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponent() const { return AbilitySystemComponent; }
	UPlayerAttributeSet* GetPlayerAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Score")
	int32 GetKills() const { return KillCount; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Score")
	int32 GetDeaths() const { return DeathCount; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch|Score")
	void AddKill();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch|Score")
	void AddDeath();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch|Score")
	void ResetMatchScore();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombatStateForRespawn();

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch|Score")
	FPlayerScoreChangedDelegate OnKillCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch|Score")
	FPlayerScoreChangedDelegate OnDeathCountChanged;

protected:
	UFUNCTION()
	void OnRep_KillCount(int32 OldKillCount);

	UFUNCTION()
	void OnRep_DeathCount(int32 OldDeathCount);

	void SetKillCount(int32 NewKillCount);
	void SetDeathCount(int32 NewDeathCount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<UPlayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_KillCount, BlueprintReadOnly, Category = "Deathmatch|Score")
	int32 KillCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_DeathCount, BlueprintReadOnly, Category = "Deathmatch|Score")
	int32 DeathCount = 0;
};
