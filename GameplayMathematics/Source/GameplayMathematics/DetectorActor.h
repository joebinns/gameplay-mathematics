
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectorActor.generated.h"

// TODO: Add method to temporarily disable the detector

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

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnDetectorActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void Shutdown();

	UFUNCTION()
	void UpdateShutdownTimer(float DeltaTime);

	UFUNCTION()
	void UpdateDetectionTimer(float DeltaTime);
	
	UFUNCTION()
	void UpdateColor();

private:
	UPROPERTY()
	APawn* Player;
	
	float TimeInCone = 0.f;
	float ShutdownTimer = 0.f;

};
