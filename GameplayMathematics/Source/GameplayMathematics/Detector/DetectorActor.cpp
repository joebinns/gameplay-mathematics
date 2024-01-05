

#include "DetectorActor.h"

#include "Components/SpotLightComponent.h"
#include "../Projectile/ProjectileActor.h"

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

	// Bind to on actor hit
	OnActorHit.AddDynamic(this, &ADetectorActor::OnDetectorActorHit);
}

void ADetectorActor::OnDetectorActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	const AProjectileActor* GameplayMathematicsProjectile = Cast<AProjectileActor>(OtherActor);
	if (GameplayMathematicsProjectile != nullptr)
	{
		Shutdown();
	}
}

void ADetectorActor::Shutdown()
{
	// Disable the detector until the shutdown timer has passed
	ShutdownTimer = ShutdownDuration;
	TimeInCone = 0.f;
	SpotLight->SetVisibility(false);
}

void ADetectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForProjectileCollision();
	
	if (ShutdownTimer > 0.f)
	{
		UpdateShutdownTimer(DeltaTime);
	}
	else
	{
		SpotLight->SetVisibility(true);
		UpdateDetectionTimer(DeltaTime);
		UpdateColor();
	}
}

void ADetectorActor::CheckForProjectileCollision()
{
	for (auto Projectile : AProjectileActor::Projectiles)
	{
		// TODO: Check if the projectile (sphere) is colliding with the detector actor (AABB)
	}
}

void ADetectorActor::UpdateShutdownTimer(float DeltaTime)
{
	ShutdownTimer -= DeltaTime;
}

void ADetectorActor::UpdateDetectionTimer(float DeltaTime)
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
}

void ADetectorActor::UpdateColor()
{
	// Update the color of the spot light based on the time in cone
	// Lerp between the warning color and the spotted color
	const float T = TimeInCone / SpottedTriggerTime;
	const FLinearColor Color = TimeInCone == 0.f ? NeutralColor : FLinearColor::LerpUsingHSV(WarningColor, SpottedColor, T);
	SpotLight->SetLightColor(Color);
}
