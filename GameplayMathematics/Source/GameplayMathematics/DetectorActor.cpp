

#include "DetectorActor.h"

#include "Components/SpotLightComponent.h"

ADetectorActor::ADetectorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));

	// Declare hierarchy
	RootComponent = Mesh;
	SpotLight->SetupAttachment(Mesh);
}

void ADetectorActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialise colour
	SpotLight->SetLightFColor(NeutralColor);
}

void ADetectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

