// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PlayerEquipmentComponent.generated.h"

class ABaseWeapon;
class UArrowDataAsset;

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

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DestroyCarriedWeapons();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	FGameplayTag GetCurrentWeaponTag() const { return CurrentWeaponTag; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	ABaseWeapon* GetWeaponByTag(FGameplayTag WeaponTag) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	ABaseWeapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment|Arrows")
	void SelectArrowByTag(FGameplayTag ArrowTag);

	UFUNCTION(BlueprintPure, Category = "Equipment|Arrows")
	FGameplayTag GetSelectedArrowTag() const { return SelectedArrowTag; }

	UFUNCTION(BlueprintPure, Category = "Equipment|Arrows")
	UArrowDataAsset* GetSelectedArrowData() const;

	UFUNCTION(BlueprintPure, Category = "Equipment|Arrows")
	UArrowDataAsset* GetArrowDataByTag(FGameplayTag ArrowTag) const;

	UFUNCTION(BlueprintPure, Category = "Equipment|Arrows")
	const TArray<UArrowDataAsset*>& GetAvailableArrowData() const { return AvailableArrowData; }

protected:
	UFUNCTION(Server, Reliable)
	void ServerSelectArrowByTag(FGameplayTag ArrowTag);

	UFUNCTION()
	void OnRep_SelectedArrowTag();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment|Arrows")
	void OnSelectedArrowChanged(UArrowDataAsset* SelectedArrowData);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	FGameplayTag CurrentWeaponTag;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	TArray<FCarriedWeaponEntry> CarriedWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Arrows")
	TArray<TObjectPtr<UArrowDataAsset>> AvailableArrowData;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedArrowTag, BlueprintReadOnly, Category = "Equipment|Arrows")
	FGameplayTag SelectedArrowTag;
};
