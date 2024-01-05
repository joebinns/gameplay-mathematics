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

	// Create sphere
	Sphere.Center = FVector::Zero();
	Sphere.W = 1.0f;
	
	// Use a sphere as a simple collision representation
	Mesh->SetWorldScale3D(FVector::One() * 25.0f);
	Mesh->BodyInstance.SetCollisionProfileName("Projectile");
	
	// Set root component
	RootComponent = Mesh;

	// Use a ProjectileMovementComponent to govern the mesh's movement
	ProjectileMovement->UpdatedComponent = Mesh;
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

void AProjectileActor::Tick()
{
	Sphere.Center = Mesh->GetComponentLocation();
	Sphere.W = Mesh->GetComponentScale().X * 50.f;
}

void AProjectileActor::Destroyed()
{
	Projectiles.Remove(this);
}