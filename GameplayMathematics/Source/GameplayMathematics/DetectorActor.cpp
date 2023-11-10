

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

	// Initialise the player reference
	Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	// Initialise spot light's colour
	SpotLight->SetLightFColor(NeutralColor);
}

void ADetectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Detect player
	DetectPlayer(DeltaTime);
}

void ADetectorActor::DetectPlayer(float DeltaTime)
{
	const FVector DisplacementToPlayer = Player->GetActorLocation() - GetActorLocation();
	const float FacingPlayerDot = FVector::DotProduct(DisplacementToPlayer.GetSafeNormal(), GetActorForwardVector());

	const bool IsPlayerInVisionCone = FacingPlayerDot >= SpotLight->GetCosHalfConeAngle() && DisplacementToPlayer.Length() <= SpotLight->AttenuationRadius;
	
	if (IsPlayerInVisionCone)
	{
		TimeInCone += DeltaTime;
	}
	else
	{
		TimeInCone -= DeltaTime;
	}

	TimeInCone = FMath::Clamp(TimeInCone, 0.f, SpottedTriggerTime);
	
	if (TimeInCone == 0.f)
	{
		SpotLight->SetLightFColor(NeutralColor);
	}
	else if (0.f < TimeInCone && TimeInCone < SpottedTriggerTime)
	{
		SpotLight->SetLightFColor(WarningColor);
	}
	else if (SpottedTriggerTime <= TimeInCone)
	{
		SpotLight->SetLightFColor(SpottedColor);
	}

}
