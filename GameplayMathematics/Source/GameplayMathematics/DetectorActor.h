
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectorActor.generated.h"

// TODO: Add initialisation of actor components
// TODO: Add detection method (to tick) -- set the color of the spotlight component based on the dot product

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
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USpotLightComponent* SpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor NeutralColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor DetectedColor;

public: 
	virtual void Tick(float DeltaTime) override;

};
