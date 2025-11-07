// CC_EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "Interfaces/CC_DeathPickupInterface.h"
#include "Utils/CC_Types.h"
#include "CC_EnemyCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class CRASHCOURSE_API ACC_EnemyCharacter : public ACC_BaseCharacter, public ICC_DeathPickupInterface
{
	GENERATED_BODY()

public:
	
	ACC_EnemyCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crash|AI")
	float AcceptanceRadius{500.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crash|AI")
	float MinAttackDelay{0.1f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crash|AI")
	float MaxAttackDelay{0.5f};

	UFUNCTION(BlueprintImplementableEvent)
	float GetTimeLineLength();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsBeingLaunched = false;

	void StopMovementUntilLanded();

	// Configure pickups directly in Blueprint!
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death Pickups")
	TArray<FPickupSpawnInfo> DeathPickups;
    
	// Interface implementation
	virtual TArray<FPickupSpawnInfo> GetDeathPickups_Implementation() const override
	{
		return DeathPickups;
	}

protected:
	
	virtual void BeginPlay() override;

	virtual void HandleDeath() override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UFUNCTION()
	void EnableMovementOnLanded(const FHitResult& Hit);
	
};