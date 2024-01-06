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

### [Intersection](https://github.com/joebinns/gameplay-mathematics/releases/tag/intersection-v2) (old)
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

### [Intersection](https://github.com/joebinns/gameplay-mathematics/releases/tag/intersection-revised) (new)
I replaced Unreal's collision system with a custom intersection check to determine if projectiles from the gun where hitting the detector actor.
Projectiles were represented as spheres and the detector actor was approximated to be an AABB.

``` cpp
void ADetectorActor::CheckCollisionWithProjectiles()
{
	NewlyCollidingProjectiles.Empty();
	CollidingProjectiles.Empty();
	IsNewlyCollidingWithProjectiles = false;
	for (const auto Projectile : AProjectileActor::Projectiles)
	{
		if (IsCollisionBetweenSphereAndAABB(Projectile->GetSphere(), CollisionAABB))
		{
			if (PreviouslyCollidingProjectiles.Contains(Projectile))
			{
				CollidingProjectiles.Add(Projectile);
				continue;
			}
			
			NewlyCollidingProjectiles.Add(Projectile);
			CollidingProjectiles.Add(Projectile);
			IsNewlyCollidingWithProjectiles = true;
		}
	}
	PreviouslyCollidingProjectiles = CollidingProjectiles;
}

bool ADetectorActor::IsCollisionBetweenSphereAndAABB(const FSphere Sphere, const FBox AABB)
{
	const auto ClosestPointInAABB = GetClosestPointInAABB(Sphere.Center, AABB);
	return IsPointInSphere(ClosestPointInAABB, Sphere);
}

FVector ADetectorActor::GetClosestPointInAABB(const FVector Point, const FBox AABB)
{
	FVector ClosestPointInAABB;
	ClosestPointInAABB.X = FMath::Max(AABB.Min.X, FMath::Min(Point.X, AABB.Max.X));
	ClosestPointInAABB.Y = FMath::Max(AABB.Min.Y, FMath::Min(Point.Y, AABB.Max.Y));
	ClosestPointInAABB.Z = FMath::Max(AABB.Min.Z, FMath::Min(Point.Z, AABB.Max.Z));
	return ClosestPointInAABB;
}

bool ADetectorActor::IsPointInSphere(const FVector Point, const FSphere Sphere)
{
	const auto DistanceBetweenPointAndCenterOfSphere = FVector::Distance(Point, Sphere.Center);
	return DistanceBetweenPointAndCenterOfSphere < Sphere.W;
}
```

### [Collision](https://github.com/joebinns/gameplay-mathematics/releases/tag/collision) (old)
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

### [Collision](https://github.com/joebinns/gameplay-mathematics/releases/tag/collision-revised) (new)
I implemented custom collision logic by freezing projectiles upon intersection, recording their existing velocity and then re-applying that velocity in reverse and re-enabling physics when the detector actor exits shutdown mode -- such as to 'shoot' at the player.

``` cpp
void ADetectorActor::FreezeProjectiles()
{
	for (auto* Projectile : NewlyCollidingProjectiles)
	{
		auto* ProjectileMesh = Projectile->GetMesh();
		FrozenProjectileToVelocity.Add(Projectile, ProjectileMesh->GetPhysicsLinearVelocity());
		ProjectileMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		ProjectileMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		ProjectileMesh->SetEnableGravity(false);
	}
}

void ADetectorActor::UnFreezeProjectiles()
{
	for (const auto FrozenProjectileAndVelocity : FrozenProjectileToVelocity)
	{
		const auto ProjectileMesh = FrozenProjectileAndVelocity.Key->GetMesh();
		ProjectileMesh->SetPhysicsLinearVelocity(-FrozenProjectileAndVelocity.Value);
		ProjectileMesh->SetEnableGravity(true);
	}
	FrozenProjectileToVelocity.Empty();
}
```

### [Noise](https://github.com/joebinns/gameplay-mathematics/releases/tag/noise)
Voronoi noise was used to create an electric effect to show when the detector is disabled.
A dynamic material instance was then used to vary the effect based on when the detector is shutdown.
