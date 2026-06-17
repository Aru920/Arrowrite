// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ARROWRITE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetAimingRotationMode(bool bIsAiming);

protected:
	virtual void BeginPlay() override;

	virtual void ApplyMovementSettings();
	void SetSprinting(bool bNewIsSprinting);
	void SetMovementSpeedMultiplier(float NewMovementSpeedMultiplier);

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewIsSprinting);

	UFUNCTION()
	void OnRep_IsSprinting();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float WalkSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float RunSpeed = 500.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MovementSpeedMultiplier = 1.0f;

	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;
};
