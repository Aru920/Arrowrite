// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectiles/ArrowProjectile.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AArrowProjectile::AArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ArrowAlignmentRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ArrowAlignmentRoot"));
	ArrowAlignmentRoot->SetupAttachment(SceneRoot);
	ArrowAlignmentRoot->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(ArrowAlignmentRoot);
	CollisionComponent->InitCapsuleSize(5.0f, 45.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(CollisionComponent);
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LaunchDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchDirectionArrow"));
	LaunchDirectionArrow->SetupAttachment(SceneRoot);
	LaunchDirectionArrow->ArrowSize = 1.5f;
	LaunchDirectionArrow->ArrowLength = 140.0f;
	LaunchDirectionArrow->bHiddenInGame = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxLaunchSpeed;
	ProjectileMovement->ProjectileGravityScale = ArrowGravityScale;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bInitialVelocityInLocalSpace = false;
	ProjectileMovement->bAutoActivate = false;
}

void AArrowProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsLaunched || !bAlignToVelocity || bHasImpacted || !ProjectileMovement || ProjectileMovement->Velocity.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(ProjectileMovement->Velocity.GetSafeNormal().Rotation());
}

void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::HandleImpact);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ThisClass::HandleProjectileStop);
}

void AArrowProjectile::LaunchArrow(FVector LaunchDirection, float Intensity)
{
	if (!ProjectileMovement)
	{
		return;
	}

	const FVector Direction = LaunchDirection.IsNearlyZero() ? GetActorForwardVector() : LaunchDirection.GetSafeNormal();
	const float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
	const float LaunchSpeed = FMath::Lerp(MinLaunchSpeed, MaxLaunchSpeed, ClampedIntensity);

	bIsLaunched = true;
	bHasImpacted = false;
	SetActorTickEnabled(true);
	SetActorRotation(Direction.Rotation());

	ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	ProjectileMovement->ProjectileGravityScale = ArrowGravityScale;
	ProjectileMovement->InitialSpeed = LaunchSpeed;
	ProjectileMovement->MaxSpeed = FMath::Max(MaxLaunchSpeed, LaunchSpeed);
	ProjectileMovement->Velocity = Direction * LaunchSpeed;
	ProjectileMovement->Activate(true);
}

void AArrowProjectile::HandleImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ProcessImpact(Hit);
}

void AArrowProjectile::HandleProjectileStop(const FHitResult& ImpactResult)
{
	ProcessImpact(ImpactResult);
}

void AArrowProjectile::ProcessImpact(const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bHasImpacted)
	{
		return;
	}

	if (Hit.GetActor() == GetOwner() || Hit.GetActor() == GetInstigator())
	{
		return;
	}

	bHasImpacted = true;

	const FVector ImpactDirection = ProjectileMovement && !ProjectileMovement->Velocity.IsNearlyZero()
		? ProjectileMovement->Velocity.GetSafeNormal()
		: GetActorForwardVector();

	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}

	SetActorTickEnabled(false);

	if (bStickOnImpact)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorLocation(Hit.ImpactPoint + ImpactDirection * StickDepth);
		SetActorRotation(ImpactDirection.Rotation());

		if (UPrimitiveComponent* HitPrimitive = Hit.GetComponent())
		{
			AttachToComponent(HitPrimitive, FAttachmentTransformRules::KeepWorldTransform);
		}

		if (ImpactLifeSpan > 0.0f)
		{
			SetLifeSpan(ImpactLifeSpan);
		}
	}

	OnArrowImpact(Hit);

	if (bDestroyOnImpact)
	{
		Destroy();
	}
}
