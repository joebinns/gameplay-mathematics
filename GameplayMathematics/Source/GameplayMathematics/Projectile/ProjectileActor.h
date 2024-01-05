// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AProjectileActor();

	static TArray<AProjectileActor*> Projectiles;
	
	USphereComponent* GetSphere() const { return Sphere; }
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
protected:
	virtual void BeginPlay();
	virtual void Destroyed();
};

