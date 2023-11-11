

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
}

void ADetectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Detect player
	DetectPlayer(DeltaTime);
}

void ADetectorActor::DetectPlayer(float DeltaTime)
{
	// Update the time in cone based on if the player is in the spotlight
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

	// Update the color of the spot light based on the time in cone
	// Lerp between the warning color and the spotted color
	const float T = TimeInCone / SpottedTriggerTime;
	const FLinearColor Color = TimeInCone == 0.f ? NeutralColor : FLinearColor::LerpUsingHSV(WarningColor, SpottedColor, T);
	SpotLight->SetLightColor(Color);
}
