#include "PhaseComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UPhaseComponent::UPhaseComponent()
{
	SetIsReplicatedByDefault(true);
}

void UPhaseComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPhaseComponent, Phase);
}

void UPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check if whoever spawned our owner has a phase component.  If so, use their phase
	if (AActor* Spawner = GetOwner()->GetOwner())
	{
		if (UPhaseComponent* SpawnerPhaseComponent = Spawner->FindComponentByClass<UPhaseComponent>())
		{
			Phase = SpawnerPhaseComponent->GetPhase();
		}
	}

	OnPhaseChanged();
}

void UPhaseComponent::CyclePhase()
{
	// Even on the client we cycle the phase immediately so that the visuals can change immediately
	Phase++;
	if (Phase > GetMaxPhase())
		Phase = 0;

	OnPhaseChanged();

	// Notify the server.  This will do nothing if we're already on the server since we just set the phase above
	SetPhaseOnServer(Phase);
}

void UPhaseComponent::SetPhaseOnServer_Implementation(int NewPhase)
{
	if (Phase != NewPhase)
	{
		Phase = NewPhase;
		OnPhaseChanged();
	}
}

void UPhaseComponent::OnRep_Phase()
{
	OnPhaseChanged();
}

void UPhaseComponent::OnPhaseChanged()
{
	// Change any static meshes associated with our owner to the material for the phase
	// Architecturally this is not really this component's responsibility, but this is a simple demo which is only
	// about the networking code, so I'm doing it this way to keep the distractions to a minimum.

	if (Phase >= PhaseMaterials.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Phase out of bounds.  Are any phase materials set?"));
		return;
	}

	UMaterialInstance* PhaseMaterial = PhaseMaterials[Phase];

	TInlineComponentArray<UMeshComponent*> MeshComponents(GetOwner());
	GetOwner()->GetComponents(MeshComponents, /*bIncludeFromChildActors*/ true);
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetMaterial(0, PhaseMaterial);
	}
}
