// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PlayerEquipmentComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARROWRITE_API UPlayerEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RegisterEquippedWeapon(FGameplayTag WeaponTag);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	FGameplayTag GetCurrentWeaponTag() const { return CurrentWeaponTag; }

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	FGameplayTag CurrentWeaponTag;
};
