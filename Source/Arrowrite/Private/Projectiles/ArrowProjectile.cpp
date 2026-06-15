// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectiles/ArrowProjectile.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

namespace
{
	constexpr ECollisionChannel ArrowTraceChannel = ECC_GameTraceChannel1;
}

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
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
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
	ProjectileMovement->bSweepCollision = true;
	ProjectileMovement->bForceSubStepping = true;
	ProjectileMovement->MaxSimulationTimeStep = 0.016f;
	ProjectileMovement->MaxSimulationIterations = 8;
}

void AArrowProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsLaunched || !bAlignToVelocity || bHasImpacted || !ProjectileMovement || ProjectileMovement->Velocity.IsNearlyZero())
	{
		return;
	}

	TraceForImpact();

	if (bHasImpacted)
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
	bHasLastTraceLocation = true;
	LastTraceLocation = GetArrowTipWorldLocation();
	SetActorTickEnabled(true);
	SetActorRotation(Direction.Rotation());

	if (CollisionComponent)
	{
		CollisionComponent->IgnoreActorWhenMoving(this, true);
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
		CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
		CollisionComponent->IgnoreActorWhenMoving(ImpactIgnoredActor.Get(), true);
	}

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

void AArrowProjectile::TraceForImpact()
{
	if (!HasAuthority() || !CollisionComponent)
	{
		return;
	}

	const FVector CurrentTraceLocation = GetArrowTipWorldLocation();
	if (!bHasLastTraceLocation)
	{
		LastTraceLocation = CurrentTraceLocation;
		bHasLastTraceLocation = true;
		return;
	}

	if (LastTraceLocation.Equals(CurrentTraceLocation, KINDA_SMALL_NUMBER))
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ArrowProjectileTrace), false, this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(GetInstigator());
	QueryParams.AddIgnoredActor(ImpactIgnoredActor.Get());

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, LastTraceLocation, CurrentTraceLocation, ArrowTraceChannel, QueryParams))
	{
		ProcessImpact(Hit);
	}

	LastTraceLocation = CurrentTraceLocation;
}

void AArrowProjectile::ProcessImpact(const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!Hit.bBlockingHit)
	{
		return;
	}

	if (bHasImpacted)
	{
		return;
	}

	const AActor* HitActor = Hit.GetActor();
	const UPrimitiveComponent* HitPrimitive = Hit.GetComponent();
	if (ShouldIgnoreImpactActor(HitActor))
	{
		return;
	}

	if (HitPrimitive == CollisionComponent || (HitPrimitive && HitPrimitive->GetOwner() == this))
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

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StickArrowToImpact(Hit, ImpactDirection);

	OnArrowImpact(Hit);
}

void AArrowProjectile::StickArrowToImpact(const FHitResult& Hit, const FVector& ImpactDirection)
{
	const FVector ImpactPoint = Hit.ImpactPoint.IsNearlyZero() ? Hit.Location : Hit.ImpactPoint;
	const FVector DesiredTipLocation = ImpactPoint + ImpactDirection.GetSafeNormal() * StickDepth;

	SetActorRotation(ImpactDirection.Rotation());

	const FVector CurrentTipLocation = GetArrowTipWorldLocation();
	SetActorLocation(GetActorLocation() + (DesiredTipLocation - CurrentTipLocation));

	if (UPrimitiveComponent* HitPrimitive = Hit.GetComponent())
	{
		AttachToComponent(HitPrimitive, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
	}

	if (ImpactLifeSpan > 0.0f)
	{
		SetLifeSpan(ImpactLifeSpan);
	}
}

FVector AArrowProjectile::GetArrowTipWorldLocation() const
{
	if (ArrowMesh && ArrowMesh->DoesSocketExist(ArrowTipSocketName))
	{
		return ArrowMesh->GetSocketLocation(ArrowTipSocketName);
	}

	return CollisionComponent ? CollisionComponent->GetComponentLocation() : GetActorLocation();
}

void AArrowProjectile::SetImpactIgnoredActor(AActor* ActorToIgnore)
{
	ImpactIgnoredActor = ActorToIgnore;

	if (CollisionComponent)
	{
		CollisionComponent->IgnoreActorWhenMoving(ActorToIgnore, true);
	}
}

bool AArrowProjectile::ShouldIgnoreImpactActor(const AActor* HitActor) const
{
	if (!HitActor)
	{
		return true;
	}

	return HitActor == this
		|| HitActor == GetOwner()
		|| HitActor == GetInstigator()
		|| HitActor == ImpactIgnoredActor.Get();
}
