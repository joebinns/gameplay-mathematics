
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectorActor.generated.h"

// TODO: Interpolate the detector colour

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
	FColor NeutralColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor DetectedColor;

	UPROPERTY()
	APawn* Player;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void DetectPlayer();

};
