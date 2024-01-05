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

	UFUNCTION()
	FSphere GetSphere() const { return Sphere; }
	
	UFUNCTION(BlueprintCallable)
	static void ClearProjectiles();
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY();
	FSphere Sphere;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};

