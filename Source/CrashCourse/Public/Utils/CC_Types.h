// CC_Types.h
#pragma once

#include "CoreMinimal.h"
#include "CC_Types.generated.h"  // ADD THIS LINE

USTRUCT(BlueprintType)
struct FPickupSpawnInfo
{
	GENERATED_BODY()

	// The pickup actor to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	TSubclassOf<AActor> PickupClass;

	// Chance to spawn (0-1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	float SpawnChance = 1.0f;

	// How many to spawn (random btwn min and max)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	int32 MinSpawnCount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	int32 MaxSpawnCount = 10;

	// Random scatter radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	float ScatterRadius = 100.0f;

	// Height offset for spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Pickup")
	float HeightOffset = 100.0f;
};