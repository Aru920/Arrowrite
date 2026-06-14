// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Weapons/WeaponAnimInstance.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UWeaponDataAsset;

UCLASS()
class ARROWRITE_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetOwningPawn(APawn* InOwningPawn);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	APawn* GetOwningPawn() const { return OwningPawn; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UWeaponDataAsset* GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
	void SetWeaponAimPoseActive(bool bShouldUseAimPose);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
	void SetWeaponAnimState(EWeaponAnimState NewAnimState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponData;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<APawn> OwningPawn;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponAimPoseActive, BlueprintReadOnly, Category = "Weapon|Animation")
	bool bWeaponAimPoseActive = false;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponAnimState, BlueprintReadOnly, Category = "Weapon|Animation")
	EWeaponAnimState WeaponAnimState = EWeaponAnimState::Idle;

private:
	void ApplyWeaponAimPoseActive();
	void ApplyWeaponAnimState();

	UFUNCTION(Server, Reliable)
	void ServerSetWeaponAimPoseActive(bool bShouldUseAimPose);

	UFUNCTION(Server, Reliable)
	void ServerSetWeaponAnimState(EWeaponAnimState NewAnimState);

	UFUNCTION()
	void OnRep_WeaponAimPoseActive();

	UFUNCTION()
	void OnRep_WeaponAnimState();
};
