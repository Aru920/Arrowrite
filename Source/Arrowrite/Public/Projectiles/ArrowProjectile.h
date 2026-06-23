// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"
#include "GameFramework/Actor.h"
#include "ArrowProjectile.generated.h"

class UProjectileMovementComponent;
class USceneComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UArrowDataAsset;
class UAbilitySystemComponent;

UCLASS()
class ARROWRITE_API AArrowProjectile : public AActor
{
	GENERATED_BODY()

public:
	AArrowProjectile();

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnRep_ReplicatedMovement() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Projectile", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	void LaunchArrow(FVector LaunchDirection, float Intensity);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Arrow")
	void SetArrowData(UArrowDataAsset* NewArrowData);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetImpactIgnoredActor(AActor* ActorToIgnore);

	UFUNCTION(BlueprintPure, Category = "Projectile")
	UCapsuleComponent* GetCollisionComponent() const { return CollisionComponent; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	UStaticMeshComponent* GetArrowMesh() const { return ArrowMesh; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	UArrowComponent* GetLaunchDirectionArrow() const { return LaunchDirectionArrow; }

	UFUNCTION(BlueprintPure, Category = "Projectile|Arrow")
	UArrowDataAsset* GetArrowData() const { return ArrowData; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStickArrow(const FHitResult& Hit, FVector_NetQuantize ActorLocation, FRotator ActorRotation, FVector_NetQuantize MeshLocation, FRotator MeshRotation);

	void ProcessImpact(const FHitResult& Hit);
	void TraceForImpact();
	void StopProjectileMovement();
	void StickArrowToImpact(const FHitResult& Hit, const FVector& ImpactDirection);
	bool ApplyDamageToHitActor(const FHitResult& Hit);
	bool ApplyStatusEffectToHitActor(const FHitResult& Hit);
	void NotifyHitConfirmed() const;
	void SendHitReactEvent(const FHitResult& Hit) const;
	UAbilitySystemComponent* GetSourceAbilitySystemComponent() const;
	void ConfigureProjectileCollision() const;
	bool TryResolveCharacterMeshHit(const FHitResult& Hit, FHitResult& ResolvedHit) const;
	void AddImpactIgnoredActor(AActor* ActorToIgnore);
	void CollectInitialIgnoredActors();
	FVector GetArrowTipWorldLocation() const;
	bool ShouldIgnoreImpactActor(const AActor* HitActor) const;
	void StartLaunchVisuals();
	void ApplyLaunchMovement(const FVector& LaunchVelocity);
	void SimulateReplicatedLaunch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnArrowImpact(const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnArrowLaunched();

	UFUNCTION()
	void OnRep_ArrowData();

	UFUNCTION()
	void OnRep_IsLaunched();

	UFUNCTION()
	void OnRep_LaunchVelocity();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USceneComponent> ArrowAlignmentRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UCapsuleComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ArrowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Debug")
	TObjectPtr<UArrowComponent> LaunchDirectionArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_ArrowData, BlueprintReadOnly, Category = "Projectile|Arrow")
	TObjectPtr<UArrowDataAsset> ArrowData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact")
	FName ArrowTipSocketName = TEXT("ArrowTip");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float StickDepth = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float ImpactLifeSpan = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float InitialShooterIgnoreRadius = 160.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Damage", meta = (ClampMin = "1.0"))
	float DamageLevel = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bAlignToVelocity = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Launch", meta = (ClampMin = "0.0"))
	float MinLaunchSpeed = 700.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Launch", meta = (ClampMin = "0.0"))
	float MaxLaunchSpeed = 2200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Launch", meta = (ClampMin = "0.0"))
	float ArrowGravityScale = 0.8f;

	UPROPERTY(ReplicatedUsing = OnRep_IsLaunched)
	bool bIsLaunched = false;

	UPROPERTY(ReplicatedUsing = OnRep_LaunchVelocity)
	FVector_NetQuantize10 ReplicatedLaunchVelocity;

	bool bHasImpacted = false;
	bool bLaunchVisualsStarted = false;
	bool bReplicatedLaunchMovementStarted = false;
	bool bHasLastTraceLocation = false;
	FVector LastTraceLocation = FVector::ZeroVector;
	TArray<TWeakObjectPtr<AActor>> ImpactIgnoredActors;
};
