#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhaseComponent.generated.h"

class UMaterialInstance;

/** 
	Represents the minimal gameplay of this demo. Each player and their projectiles can be in one of two phases.If
	a projectile's phase matches its target then damage is done, otherwise not.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JTSAMPLE_API UPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhaseComponent();

	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const;

	int GetPhase() { return Phase; }

	int GetMaxPhase() { return FMath::Max(PhaseMaterials.Num() - 1, 0); }

	void CyclePhase();

protected:
	virtual void BeginPlay() override;

private:
	void OnPhaseChanged();

	UFUNCTION()
	void OnRep_Phase();

	UFUNCTION(Server, Reliable)
	void SetPhaseOnServer(int NewPhase);

	UPROPERTY(ReplicatedUsing = OnRep_Phase)
	int Phase;
	
	UPROPERTY(EditDefaultsOnly, Category = "Phase")
	TArray<UMaterialInstance*> PhaseMaterials;
};
