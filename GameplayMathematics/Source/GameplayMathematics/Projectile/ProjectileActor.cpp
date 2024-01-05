// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

TArray<AProjectileActor*> AProjectileActor::Projectiles;

AProjectileActor::AProjectileActor() 
{
	// Use a sphere as a simple collision representation
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->InitSphereRadius(5.0f);
	Sphere->BodyInstance.SetCollisionProfileName("Projectile");

	// Players can't walk on it
	Sphere->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	Sphere->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = Sphere;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = Sphere;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AProjectileActor::BeginPlay()
{
	Projectiles.AddUnique(this);
}

void AProjectileActor::Destroyed()
{
	Projectiles.Remove(this);
}