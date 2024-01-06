// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

TArray<AProjectileActor*> AProjectileActor::Projectiles;

AProjectileActor::AProjectileActor() 
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Create components
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	// Use a sphere as a simple collision representation
	Mesh->SetSimulatePhysics(true);
	Mesh->SetWorldScale3D(FVector::One() * 0.05f);
	Mesh->BodyInstance.SetCollisionProfileName("Projectile");
	Mesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	Mesh->CanCharacterStepUpOn = ECB_No;
	
	// Set root component
	RootComponent = Mesh;

	// Use a ProjectileMovementComponent to govern the mesh's movement
	ProjectileMovement->UpdatedComponent = Mesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 10 seconds by default
	InitialLifeSpan = 10.0f;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	Projectiles.AddUnique(this);
}

void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Sphere.Center = Mesh->GetComponentLocation();
	Sphere.W = Mesh->GetComponentScale().X * 50.f;
}

void AProjectileActor::Destroyed()
{
	Super::Destroyed();
	Projectiles.Remove(this);
}

void AProjectileActor::ClearProjectiles()
{
	Projectiles.Empty();
}
