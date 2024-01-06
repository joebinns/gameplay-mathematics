
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectorActor.generated.h"

class USpotLightComponent;

UCLASS()
class GAMEPLAYMATHEMATICS_API ADetectorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADetectorActor();

protected: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USpotLightComponent* SpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor NeutralColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor WarningColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor SpottedColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpottedTriggerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShutdownDuration;

	UPROPERTY(BlueprintReadOnly)
	float ShutdownTimer = 0.f;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void CheckCollisionWithProjectiles();
	
	UFUNCTION()
	bool IsCollisionBetweenSphereAndAABB(const FSphere Sphere, const FBox AABB);

	UFUNCTION()
	FVector GetClosestPointInAABB(const FVector Point, const FBox AABB);

	UFUNCTION()
	bool IsPointInSphere(const FVector Point, const FSphere Sphere);
	
	UFUNCTION()
	void Shutdown();

	UFUNCTION()
	void UpdateShutdownTimer(float DeltaTime);

	UFUNCTION()
	void UpdateDetectionTimer(float DeltaTime);
	
	UFUNCTION()
	void UpdateColor();

	UFUNCTION()
	bool GetIsShutdown();

private:
	UPROPERTY()
	APawn* Player;

	TArray<AProjectileActor*> PreviouslyCollidingProjectiles;
	
	FBox CollisionAABB;
	
	float TimeInCone = 0.f;

	bool IsCollidingWithProjectiles = false;

};
