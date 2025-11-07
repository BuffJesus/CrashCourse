// CC_DeathPickupInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utils/CC_Types.h"  // ADD THIS - include instead of forward declare
#include "CC_DeathPickupInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCC_DeathPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class ICC_DeathPickupInterface
{
	GENERATED_BODY()

public:
	// Get the pickup configuration for this actor
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Death Pickups")
	TArray<FPickupSpawnInfo> GetDeathPickups() const;
};