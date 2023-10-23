#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JTSAMPLE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintGetter)
	float GetHealth() { return CurrentHealth; }

	UFUNCTION(BlueprintSetter)
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	float AdjustHealth(float IncreaseBy);

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDied OnDied;

private:
	void FireChangeEvents();

	UFUNCTION()
	void OnRep_CurrentHealth();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintGetter = GetHealth, BlueprintSetter = SetHealth)
	float CurrentHealth;

	UPROPERTY()
	bool Dead = false;		
};
