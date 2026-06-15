// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScalableFloat.h"
#include "ArrowProjectile.generated.h"

class UProjectileMovementComponent;
class USceneComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UGameplayEffect;

UCLASS()
class ARROWRITE_API AArrowProjectile : public AActor
{
	GENERATED_BODY()

public:
	AArrowProjectile();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	void LaunchArrow(FVector LaunchDirection, float Intensity);

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

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStickArrow(const FHitResult& Hit, FVector_NetQuantizeNormal ImpactDirection);

	void ProcessImpact(const FHitResult& Hit);
	void TraceForImpact();
	void StopProjectileMovement();
	void StickArrowToImpact(const FHitResult& Hit, const FVector& ImpactDirection);
	bool ApplyDamageToHitActor(const FHitResult& Hit);
	void SendHitReactEvent(const FHitResult& Hit) const;
	void ConfigureProjectileCollision() const;
	bool TryResolveCharacterMeshHit(const FHitResult& Hit, FHitResult& ResolvedHit) const;
	void AddImpactIgnoredActor(AActor* ActorToIgnore);
	void CollectInitialIgnoredActors();
	FVector GetArrowTipWorldLocation() const;
	bool ShouldIgnoreImpactActor(const AActor* HitActor) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnArrowImpact(const FHitResult& Hit);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact")
	FName ArrowTipSocketName = TEXT("ArrowTip");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float StickDepth = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float ImpactLifeSpan = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0.0"))
	float InitialShooterIgnoreRadius = 160.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Damage")
	FScalableFloat BaseDamage;

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

	bool bIsLaunched = false;
	bool bHasImpacted = false;
	bool bHasLastTraceLocation = false;
	FVector LastTraceLocation = FVector::ZeroVector;
	TArray<TWeakObjectPtr<AActor>> ImpactIgnoredActors;
};
