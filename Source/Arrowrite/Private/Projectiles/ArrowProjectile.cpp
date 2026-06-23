// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectiles/ArrowProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BlueprintFunctionLibraries/CombatBlueprintLibrary.h"
#include "Character/Player/PlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"
#include "Player/GamePlayerController.h"
#include "Projectiles/ArrowDataAsset.h"
#include "Tags/GameplayTags.h"

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
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(30.0f);
	NetPriority = 3.0f;

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
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
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
	ProjectileMovement->UpdatedComponent = SceneRoot;
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

void AArrowProjectile::OnRep_ReplicatedMovement()
{
	if (bHasImpacted)
	{
		return;
	}

	Super::OnRep_ReplicatedMovement();
}

void AArrowProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArrowProjectile, ArrowData);
	DOREPLIFETIME(AArrowProjectile, bIsLaunched);
	DOREPLIFETIME(AArrowProjectile, ReplicatedLaunchVelocity);
}

void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();

	ConfigureProjectileCollision();

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
	const FVector LaunchVelocity = Direction * LaunchSpeed;

	ReplicatedLaunchVelocity = FVector_NetQuantize10(LaunchVelocity);
	bIsLaunched = true;
	bHasImpacted = false;
	bLaunchVisualsStarted = false;
	bReplicatedLaunchMovementStarted = false;
	bHasLastTraceLocation = true;
	LastTraceLocation = GetArrowTipWorldLocation();
	CollectInitialIgnoredActors();
	ApplyLaunchMovement(LaunchVelocity);

	StartLaunchVisuals();
	ForceNetUpdate();
}

void AArrowProjectile::SetArrowData(UArrowDataAsset* NewArrowData)
{
	ArrowData = NewArrowData;
	if (HasAuthority())
	{
		ForceNetUpdate();
	}
}

void AArrowProjectile::OnRep_ArrowData()
{
	SimulateReplicatedLaunch();
	StartLaunchVisuals();
}

void AArrowProjectile::OnRep_IsLaunched()
{
	SimulateReplicatedLaunch();
	StartLaunchVisuals();
}

void AArrowProjectile::OnRep_LaunchVelocity()
{
	SimulateReplicatedLaunch();
}

void AArrowProjectile::StartLaunchVisuals()
{
	if (bLaunchVisualsStarted || !bIsLaunched || bHasImpacted)
	{
		return;
	}

	if (!ArrowData && !HasAuthority())
	{
		return;
	}

	bLaunchVisualsStarted = true;
	OnArrowLaunched();
}

void AArrowProjectile::ApplyLaunchMovement(const FVector& LaunchVelocity)
{
	if (!ProjectileMovement || LaunchVelocity.IsNearlyZero())
	{
		return;
	}

	const FVector Direction = LaunchVelocity.GetSafeNormal();
	const float LaunchSpeed = LaunchVelocity.Size();

	bHasLastTraceLocation = true;
	LastTraceLocation = GetArrowTipWorldLocation();
	SetActorTickEnabled(true);
	SetActorRotation(Direction.Rotation());

	if (CollisionComponent)
	{
		if (HasAuthority())
		{
			ConfigureProjectileCollision();
			CollisionComponent->IgnoreActorWhenMoving(this, true);
			for (const TWeakObjectPtr<AActor>& IgnoredActor : ImpactIgnoredActors)
			{
				CollisionComponent->IgnoreActorWhenMoving(IgnoredActor.Get(), true);
			}
		}
		else
		{
			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	ProjectileMovement->SetUpdatedComponent(SceneRoot);
	ProjectileMovement->ProjectileGravityScale = ArrowGravityScale;
	ProjectileMovement->InitialSpeed = LaunchSpeed;
	ProjectileMovement->MaxSpeed = FMath::Max(MaxLaunchSpeed, LaunchSpeed);
	ProjectileMovement->Velocity = LaunchVelocity;
	ProjectileMovement->Activate(true);
}

void AArrowProjectile::SimulateReplicatedLaunch()
{
	if (HasAuthority() || !bIsLaunched || bHasImpacted || bReplicatedLaunchMovementStarted || ReplicatedLaunchVelocity.IsNearlyZero())
	{
		return;
	}

	bReplicatedLaunchMovementStarted = true;
	ApplyLaunchMovement(FVector(ReplicatedLaunchVelocity));
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
	for (const TWeakObjectPtr<AActor>& IgnoredActor : ImpactIgnoredActors)
	{
		QueryParams.AddIgnoredActor(IgnoredActor.Get());
	}

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

	FHitResult ImpactHit = Hit;
	TryResolveCharacterMeshHit(Hit, ImpactHit);

	const AActor* HitActor = ImpactHit.GetActor();
	const UPrimitiveComponent* HitPrimitive = ImpactHit.GetComponent();
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

	StopProjectileMovement();
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	StickArrowToImpact(ImpactHit, ImpactDirection);
	SetReplicateMovement(false);
	ForceNetUpdate();

	const FVector MeshLocation = ArrowMesh ? ArrowMesh->GetComponentLocation() : GetActorLocation();
	const FRotator MeshRotation = ArrowMesh ? ArrowMesh->GetComponentRotation() : GetActorRotation();

	MulticastStickArrow(ImpactHit, GetActorLocation(), GetActorRotation(), MeshLocation, MeshRotation);
	const bool bAppliedDamage = ApplyDamageToHitActor(ImpactHit);
	ApplyStatusEffectToHitActor(ImpactHit);

	if (bAppliedDamage)
	{
		NotifyHitConfirmed();
		SendHitReactEvent(ImpactHit);
	}
}

void AArrowProjectile::MulticastStickArrow_Implementation(const FHitResult& Hit, FVector_NetQuantize ActorLocation, FRotator ActorRotation, FVector_NetQuantize MeshLocation, FRotator MeshRotation)
{
	bHasImpacted = true;
	bReplicatedLaunchMovementStarted = false;
	StopProjectileMovement();

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorLocationAndRotation(FVector(ActorLocation), ActorRotation, false, nullptr, ETeleportType::TeleportPhysics);

	if (ArrowMesh)
	{
		ArrowMesh->SetWorldLocationAndRotation(FVector(MeshLocation), MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (UPrimitiveComponent* HitPrimitive = Hit.GetComponent())
	{
		AttachToComponent(HitPrimitive, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
	}

	if (ImpactLifeSpan > 0.0f)
	{
		SetLifeSpan(ImpactLifeSpan);
	}

	SetReplicateMovement(false);
	OnArrowImpact(Hit);
}

void AArrowProjectile::StopProjectileMovement()
{
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
		ProjectileMovement->SetComponentTickEnabled(false);
		ProjectileMovement->Velocity = FVector::ZeroVector;
		ProjectileMovement->SetUpdatedComponent(nullptr);
	}

	SetActorTickEnabled(false);
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

bool AArrowProjectile::ApplyDamageToHitActor(const FHitResult& Hit)
{
	if (!HasAuthority() || !ArrowData || !ArrowData->DamageEffectClass)
	{
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (ShouldIgnoreImpactActor(HitActor))
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC)
	{
		return false;
	}

	UAbilitySystemComponent* SourceASC = GetSourceAbilitySystemComponent();
	if (!SourceASC)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(GetInstigator(), this);
	EffectContext.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ArrowData->DamageEffectClass, DamageLevel, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return false;
	}

	const float Damage = ArrowData->BaseDamage.GetValueAtLevel(DamageLevel);
	SpecHandle.Data->SetSetByCallerMagnitude(ArrowriteGameplayTags::Shared_SetByCaller_BaseDamage, Damage);

	if (APlayerCharacter* HitPlayerCharacter = Cast<APlayerCharacter>(HitActor))
	{
		AActor* SourceActor = GetInstigator() ? Cast<AActor>(GetInstigator()) : GetOwner();
		HitPlayerCharacter->SetLastHitReactDirection(UCombatBlueprintLibrary::CalculateHitReactDirection(HitPlayerCharacter, SourceActor));
	}

	const FActiveGameplayEffectHandle ActiveEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	return ActiveEffectHandle.WasSuccessfullyApplied();
}

void AArrowProjectile::NotifyHitConfirmed() const
{
	AController* SourceController = GetInstigatorController();
	if (!SourceController)
	{
		if (const APawn* InstigatorPawn = GetInstigator())
		{
			SourceController = InstigatorPawn->GetController();
		}
	}

	if (!SourceController)
	{
		if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
		{
			SourceController = OwnerPawn->GetController();
		}
		else
		{
			SourceController = Cast<AController>(GetOwner());
		}
	}

	if (AGamePlayerController* PlayerController = Cast<AGamePlayerController>(SourceController))
	{
		PlayerController->ClientNotifyConfirmedHit();
	}
}

bool AArrowProjectile::ApplyStatusEffectToHitActor(const FHitResult& Hit)
{
	if (!HasAuthority() || !ArrowData || !ArrowData->StatusEffectClass)
	{
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (ShouldIgnoreImpactActor(HitActor))
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	UAbilitySystemComponent* SourceASC = GetSourceAbilitySystemComponent();
	if (!TargetASC || !SourceASC)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(GetInstigator(), this);
	EffectContext.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ArrowData->StatusEffectClass, DamageLevel, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return false;
	}

	const float StatusDamage = ArrowData->StatusDamage.GetValueAtLevel(DamageLevel);
	SpecHandle.Data->SetSetByCallerMagnitude(ArrowriteGameplayTags::Shared_SetByCaller_BaseDamage, StatusDamage);

	const FActiveGameplayEffectHandle ActiveEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	return ActiveEffectHandle.WasSuccessfullyApplied();
}

UAbilitySystemComponent* AArrowProjectile::GetSourceAbilitySystemComponent() const
{
	if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator()))
	{
		return SourceASC;
	}

	if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		return SourceASC;
	}

	for (const TWeakObjectPtr<AActor>& IgnoredActor : ImpactIgnoredActors)
	{
		if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(IgnoredActor.Get()))
		{
			return SourceASC;
		}
	}

	return nullptr;
}

void AArrowProjectile::SendHitReactEvent(const FHitResult& Hit) const
{
	if (!HasAuthority())
	{
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor || ShouldIgnoreImpactActor(HitActor))
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = ArrowriteGameplayTags::Event_Player_HitReact;
	EventData.Instigator = GetInstigator() ? GetInstigator() : GetOwner();
	EventData.Target = HitActor;
	EventData.OptionalObject = this;
	EventData.ContextHandle.AddHitResult(Hit);

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
	{
		TargetASC->HandleGameplayEvent(ArrowriteGameplayTags::Event_Player_HitReact, &EventData);
	}
}

void AArrowProjectile::ConfigureProjectileCollision() const
{
	if (!CollisionComponent)
	{
		return;
	}

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

bool AArrowProjectile::TryResolveCharacterMeshHit(const FHitResult& Hit, FHitResult& ResolvedHit) const
{
	const UCapsuleComponent* HitCapsule = Cast<UCapsuleComponent>(Hit.GetComponent());
	const ACharacter* HitCharacter = Cast<ACharacter>(Hit.GetActor());
	if (!HitCapsule || !HitCharacter || HitCapsule != HitCharacter->GetCapsuleComponent())
	{
		return false;
	}

	USkeletalMeshComponent* CharacterMesh = HitCharacter->GetMesh();
	if (!CharacterMesh)
	{
		return false;
	}

	const FVector TraceStart = Hit.TraceStart.IsNearlyZero() ? LastTraceLocation : FVector(Hit.TraceStart);
	const FVector TraceEnd = Hit.TraceEnd.IsNearlyZero() ? GetArrowTipWorldLocation() : FVector(Hit.TraceEnd);
	if (TraceStart.Equals(TraceEnd, KINDA_SMALL_NUMBER))
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ArrowProjectileCharacterMeshTrace), false, this);
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult MeshHit;
	if (!CharacterMesh->LineTraceComponent(MeshHit, TraceStart, TraceEnd, QueryParams))
	{
		return false;
	}

	MeshHit.HitObjectHandle = Hit.HitObjectHandle;
	MeshHit.Component = CharacterMesh;
	MeshHit.TraceStart = Hit.TraceStart;
	MeshHit.TraceEnd = Hit.TraceEnd;
	ResolvedHit = MeshHit;
	return true;
}

void AArrowProjectile::SetImpactIgnoredActor(AActor* ActorToIgnore)
{
	AddImpactIgnoredActor(ActorToIgnore);
}

void AArrowProjectile::AddImpactIgnoredActor(AActor* ActorToIgnore)
{
	if (!ActorToIgnore || ActorToIgnore == this)
	{
		return;
	}

	for (const TWeakObjectPtr<AActor>& IgnoredActor : ImpactIgnoredActors)
	{
		if (IgnoredActor.Get() == ActorToIgnore)
		{
			return;
		}
	}

	ImpactIgnoredActors.Add(ActorToIgnore);

	if (CollisionComponent)
	{
		CollisionComponent->IgnoreActorWhenMoving(ActorToIgnore, true);
	}
}

void AArrowProjectile::CollectInitialIgnoredActors()
{
	AddImpactIgnoredActor(GetOwner());
	AddImpactIgnoredActor(GetInstigator());

	UWorld* World = GetWorld();
	if (!World || InitialShooterIgnoreRadius <= 0.0f)
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ArrowProjectileInitialIgnore), false, this);
	TArray<FOverlapResult> OverlapResults;
	const bool bHasOverlaps = World->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(InitialShooterIgnoreRadius),
		QueryParams
	);

	if (!bHasOverlaps)
	{
		return;
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AddImpactIgnoredActor(OverlapResult.GetActor());
	}
}

bool AArrowProjectile::ShouldIgnoreImpactActor(const AActor* HitActor) const
{
	if (!HitActor)
	{
		return true;
	}

	if (HitActor == this)
	{
		return true;
	}

	for (const TWeakObjectPtr<AActor>& IgnoredActor : ImpactIgnoredActors)
	{
		if (IgnoredActor.Get() == HitActor)
		{
			return true;
		}
	}

	return false;
}
