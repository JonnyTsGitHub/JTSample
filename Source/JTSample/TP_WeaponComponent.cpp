// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "JTSampleCharacter.h"
#include "JTSampleProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	SetIsReplicatedByDefault(true);

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTP_WeaponComponent, Character);
}

void UTP_WeaponComponent::Fire()
{
	if(Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}


	// If it hasn't been long enough since the last fire, ignore the firing command.  This is filtered on the client
	// to avoid generating too much network traffic/overflowing the buffer
	FDateTime Now = FDateTime::Now();
	if (LastFireTime + FiringDelay > Now)
	{
		return;
	}

	LastFireTime = Now;

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		SpawnProjectile();
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::SpawnProjectile_Implementation()
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Instigator = Character->GetInstigator();
			ActorSpawnParams.Owner = Character;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AJTSampleProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
}

void UTP_WeaponComponent::CharacterDestroyed(AActor* InActor)
{
	// If the character holding us is destroyed, destroy ourselves.  In the future this could instead drop to the ground.
	GetOwner()->Destroy();
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindCharacterEvents();
}

void UTP_WeaponComponent::AttachWeapon(AJTSampleCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if(Character != nullptr)
	{
		// Attach the weapon to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		GetOwner()->AttachToComponent(Character->GetMesh1P(),AttachmentRules, FName(TEXT("GripPoint")));

		// Set our actor to be owned by the target character for replication reasons
		GetOwner()->SetOwner(Character);

		BindCharacterEvents();
	}
}

void UTP_WeaponComponent::BindCharacterEvents()
{
	// Unregister the old character event
	UnbindCharacterEvents();

	// Register the current character
	if (Character != nullptr)
	{
		Character->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
		Character->OnDestroyed.AddDynamic(this, &UTP_WeaponComponent::CharacterDestroyed);
		BoundCharacter = Character;
	}
}

void UTP_WeaponComponent::UnbindCharacterEvents()
{
	if (BoundCharacter != nullptr)
	{
		BoundCharacter->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
		Character->OnDestroyed.RemoveDynamic(this, &UTP_WeaponComponent::CharacterDestroyed);
		BoundCharacter = nullptr;
	}
}

void UTP_WeaponComponent::OnRep_Character()
{
	BindCharacterEvents();
}
