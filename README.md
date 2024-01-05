# Gameplay Mathematics
An assignment made for Futuregames course "3D Mathematics".

## Controls
### Movement
Use <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd> and <kbd>D</kbd> to move.<br>
Use <kbd>Space</kbd> to jump.

### Shooting
Walk into the gun trigger to pick up the gun.<br>
Whilst holding the gun, use <kbd>LMB</kbd> to shoot.

## Features
### [Context](https://github.com/joebinns/gameplay-mathematics/releases/tag/context)
Calculated the displacement of the player from a detector.
The displacement's magnitude was used to determine if the player was in range of the detector.
The dot product of the displacement and the detector's forward vector were used to check if the player was in the detector's cone of vision.

``` cpp
const FVector DisplacementToPlayer = Player->GetActorLocation() - GetActorLocation();
const float FacingPlayerDot = FVector::DotProduct(DisplacementToPlayer.GetSafeNormal(), GetActorForwardVector());
const bool IsPlayerInVisionCone = FacingPlayerDot >= SpotLight->GetCosHalfConeAngle() && DisplacementToPlayer.Length() <= SpotLight->AttenuationRadius;
```

### [Interpolation](https://github.com/joebinns/gameplay-mathematics/releases/tag/interpolation)
The time the player had spent in the detector's cone of vision was calculated.
The colour of the detector's spot light was then interpolated between the warning colour and the spotted colour using the normalised time.

``` cpp
const float T = TimeInCone / SpottedTriggerTime;
const FLinearColor Color = TimeInCone == 0.f ? NeutralColor : FLinearColor::LerpUsingHSV(WarningColor, SpottedColor, T);
```

### [Intersection](https://github.com/joebinns/gameplay-mathematics/releases/tag/intersection-v2)
A crosshair was added to help guide the player's aim.
A line trace was then used to determine if the player was looking at the detector actor.
The crosshair image was then dynamically switched between based on if the player was aiming at the detector actor.

``` cpp
bool AGameplayMathematicsCharacter::IsLookingAtEnemy()
{
	auto Camera = GetFirstPersonCameraComponent();

  FHitResult Hit;

  auto TraceStart = Camera->GetComponentLocation();
  auto TraceEnd = TraceStart + Camera->GetForwardVector() * LookingAtTraceRange;
	
  FCollisionQueryParams QueryParams;
  QueryParams.AddIgnoredActor(this);
	
  GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, LookingAtTraceChannel, QueryParams);

	if (!Hit.bBlockingHit) return false;
	auto ActorHit = Hit.GetActor();
	if (!IsValid(ActorHit)) return false;
	if (Cast<ADetectorActor>(ActorHit) == nullptr) return false;
	return true;
}
```

### [Collision](https://github.com/joebinns/gameplay-mathematics/releases/tag/intersection)
Hit events on the detector actor with projectiles were used to disable the detector's spot light and to set a shutdown timer on the detector.
Until the shutdown timer has passed, the detector is disabled.

``` cpp
void ADetectorActor::BeginPlay()
{
  ...
  // Bind to on actor hit
  OnActorHit.AddDynamic(this, &ADetectorActor::OnDetectorActorHit);
}

void ADetectorActor::OnDetectorActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
  const AGameplayMathematicsProjectile* GameplayMathematicsProjectile = Cast<AGameplayMathematicsProjectile>(OtherActor);
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
  ...
  if (ShutdownTimer > 0.f)
  {
    UpdateShutdownTimer(DeltaTime);
  }
  ...
}
```

### [Noise](https://github.com/joebinns/gameplay-mathematics/releases/tag/noise)
Voronoi noise was used to create an electric effect to show when the detector is disabled.
A dynamic material instance was then used to vary the effect based on when the detector is shutdown.
