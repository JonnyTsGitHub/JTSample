#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);

	MaxHealth = 100;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::FireChangeEvents()
{
	OnHealthChanged.Broadcast();

	if (CurrentHealth <= 0 && !Dead)
	{
		Dead = true;
		OnDied.Broadcast();
	}
}

void UHealthComponent::SetHealth(float NewHealth)
{	
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("SetHealth is only valid on the Server"));
		return ;
	}

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

	if (OldHealth != CurrentHealth)
		FireChangeEvents();
}

float UHealthComponent::AdjustHealth(float IncreaseBy)
{
	float OldHealth = CurrentHealth;

	SetHealth(CurrentHealth + IncreaseBy);

	// Return the actual change
	return CurrentHealth - OldHealth;
}

void UHealthComponent::OnRep_CurrentHealth()
{
	FireChangeEvents();
}

