// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

class ABaseCharacter;
class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class ARROWRITE_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Character")
	TObjectPtr<ABaseCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Character")
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	float Direction = 0.0f;

private:
	void CacheCharacterReferences();
	void UpdateMovementValues();
	static float CalculateMovementDirection(const ACharacter* InCharacter, const FVector& Velocity);
};
