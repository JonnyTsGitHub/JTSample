// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_WeaponComponent.generated.h"

class AJTSampleCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JTSAMPLE_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AJTSampleProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** Seconds between shots - be careful setting this too low which could create a lot of network traffic and/or overlow buffers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FTimespan FiringDelay = FTimespan::FromSeconds(0.2);

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AJTSampleCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		
	UFUNCTION(Server, Reliable)
	void SpawnProjectile();

private:
	UFUNCTION()
	void CharacterDestroyed(AActor* InActor);

	UFUNCTION()
	void OnRep_Character();

	void BindCharacterEvents();
	void UnbindCharacterEvents();

	/** The Character holding this weapon*/
	UPROPERTY(ReplicatedUsing = OnRep_Character)
	AJTSampleCharacter* Character;

	/** The character whose events we are currently bound to */
	UPROPERTY()
	AJTSampleCharacter* BoundCharacter;

	/** The last time this weapon was fired - used to apply the firing delay */
	FDateTime LastFireTime = FDateTime::MinValue();
};
