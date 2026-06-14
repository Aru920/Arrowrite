// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/BaseAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class ARROWRITE_API UPlayerAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Animation|Bow")
	void SetBowAimPoseActive(bool bShouldUseBowAimPose);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Bow")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Bow")
	bool bUseBowAimPose = false;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Bow")
	float AimYaw = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Bow")
	float AimPitch = 0.0f;

private:
	void UpdateAimValues();
};
