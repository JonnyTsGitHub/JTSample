// Copyright Epic Games, Inc. All Rights Reserved.

#include "JTSampleProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "PhaseComponent.h"

AJTSampleProjectile::AJTSampleProjectile() 
{
	bReplicates = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AJTSampleProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	PhaseComponent = CreateDefaultSubobject<UPhaseComponent>(TEXT("PhaseComponent"));

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AJTSampleProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		// We can only damage actors with phase components and only if the phase matches
		if (UPhaseComponent* OtherPhaseComponent = OtherActor->FindComponentByClass<UPhaseComponent>())
		{
			if (OtherPhaseComponent->GetPhase() == PhaseComponent->GetPhase())
			{
				if (APawn* DamageInstigator = GetInstigator())
				{
					float damage = GetVelocity().Length() / 200;
					UGameplayStatics::ApplyPointDamage(OtherActor, damage, NormalImpulse, Hit, DamageInstigator->Controller, this, UDamageType::StaticClass());

					// If we damaged something, destroy ourselves
					Destroy();
				}
			}
		}
		else
		{
			// We can still knock around the cubes - they've been set to replicate
			if (OtherActor != this && OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
			{
				OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

				Destroy();
			}
		}
	}
}