

#include "DetectorActor.h"

#include "Components/SpotLightComponent.h"
#include "../Projectile/ProjectileActor.h"
#include "Components/SphereComponent.h"

ADetectorActor::ADetectorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));

	// Disable collision
	Mesh->BodyInstance.SetCollisionProfileName("NoCollision");
	
	// Declare hierarchy
	RootComponent = Mesh;
	SpotLight->SetupAttachment(Mesh);
}

void ADetectorActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialise the player reference
	Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	// Initialise collision AABB
	CollisionAABB = FBox::BuildAABB(Mesh->GetComponentLocation(), Mesh->Bounds.BoxExtent);
}

void ADetectorActor::Shutdown()
{
	// Disable the detector until the shutdown timer has passed
	ShutdownTimer = ShutdownDuration;
	TimeInCone = 0.f;
	SpotLight->SetVisibility(false);
}

void ADetectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckCollisionWithProjectiles();
	
	if (IsCollidingWithProjectiles)
	{
		if (!GetIsShutdown())
		{
			Shutdown();
		}
	}

	for (const auto Projectile : CollidingProjectiles)
	{
		const auto ProjectileMesh = Projectile->GetMesh();
		ProjectileMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		ProjectileMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		ProjectileMesh->SetEnableGravity(false);
		ProjectileMesh->SetCollisionProfileName("NoCollision");
	}
	
	if (GetIsShutdown())
	{
		UpdateShutdownTimer(DeltaTime);
	}
	else
	{
		SpotLight->SetVisibility(true);
		UpdateDetectionTimer(DeltaTime);
		UpdateColor();
	}

	// Debug
	DrawDebugBox(GetWorld(), CollisionAABB.GetCenter(), CollisionAABB.GetExtent(), FColor::Green, false, 0.1f, 0, 1.f);
}

void ADetectorActor::CheckCollisionWithProjectiles()
{
	CollidingProjectiles.Empty();
	IsCollidingWithProjectiles = false;
	for (const auto Projectile : AProjectileActor::Projectiles)
	{
		if (PreviouslyCollidingProjectiles.Contains(Projectile)) continue;
		
		if (IsCollisionBetweenSphereAndAABB(Projectile->GetSphere(), CollisionAABB))
		{
			CollidingProjectiles.Add(Projectile);
			IsCollidingWithProjectiles = true;
		}
	}
	PreviouslyCollidingProjectiles.Empty();
	PreviouslyCollidingProjectiles = CollidingProjectiles;
}

bool ADetectorActor::IsCollisionBetweenSphereAndAABB(const FSphere Sphere, const FBox AABB)
{
	const auto ClosestPointInAABB = GetClosestPointInAABB(Sphere.Center, AABB);
	return IsPointInSphere(ClosestPointInAABB, Sphere);
}

FVector ADetectorActor::GetClosestPointInAABB(const FVector Point, const FBox AABB)
{
	FVector ClosestPointInAABB;
	ClosestPointInAABB.X = FMath::Max(AABB.Min.X, FMath::Min(Point.X, AABB.Max.X));
	ClosestPointInAABB.Y = FMath::Max(AABB.Min.Y, FMath::Min(Point.Y, AABB.Max.Y));
	ClosestPointInAABB.Z = FMath::Max(AABB.Min.Z, FMath::Min(Point.Z, AABB.Max.Z));
	return ClosestPointInAABB;
}

bool ADetectorActor::IsPointInSphere(const FVector Point, const FSphere Sphere)
{
	const auto DistanceBetweenPointAndCenterOfSphere = FVector::Distance(Point, Sphere.Center);
	return DistanceBetweenPointAndCenterOfSphere < Sphere.W;
}

void ADetectorActor::UpdateShutdownTimer(float DeltaTime)
{
	ShutdownTimer -= DeltaTime;
}

void ADetectorActor::UpdateDetectionTimer(float DeltaTime)
{
	// Update the time in cone based on if the player is in the spotlight
	const FVector DisplacementToPlayer = Player->GetActorLocation() - GetActorLocation();
	const float FacingPlayerDot = FVector::DotProduct(DisplacementToPlayer.GetSafeNormal(), GetActorForwardVector());
	const bool IsPlayerInVisionCone = FacingPlayerDot >= SpotLight->GetCosHalfConeAngle() && DisplacementToPlayer.Length() <= SpotLight->AttenuationRadius;
	if (IsPlayerInVisionCone)
	{
		TimeInCone += DeltaTime;
	}
	else
	{
		TimeInCone -= DeltaTime;
	}
	TimeInCone = FMath::Clamp(TimeInCone, 0.f, SpottedTriggerTime);
}

void ADetectorActor::UpdateColor()
{
	// Update the color of the spot light based on the time in cone
	// Lerp between the warning color and the spotted color
	const float T = TimeInCone / SpottedTriggerTime;
	const FLinearColor Color = TimeInCone == 0.f ? NeutralColor : FLinearColor::LerpUsingHSV(WarningColor, SpottedColor, T);
	SpotLight->SetLightColor(Color);
}

bool ADetectorActor::GetIsShutdown()
{
	return ShutdownTimer > 0.f;
}
