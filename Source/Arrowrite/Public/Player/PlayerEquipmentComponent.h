// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PlayerEquipmentComponent.generated.h"

class ABaseWeapon;

USTRUCT(BlueprintType)
struct FCarriedWeaponEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<ABaseWeapon> Weapon = nullptr;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARROWRITE_API UPlayerEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RegisterEquippedWeapon(ABaseWeapon* Weapon);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	FGameplayTag GetCurrentWeaponTag() const { return CurrentWeaponTag; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	ABaseWeapon* GetWeaponByTag(FGameplayTag WeaponTag) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	ABaseWeapon* GetCurrentWeapon() const;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	FGameplayTag CurrentWeaponTag;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	TArray<FCarriedWeaponEntry> CarriedWeapons;
};
