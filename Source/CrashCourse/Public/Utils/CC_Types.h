// CC_Types.h
#pragma once

#include "CoreMinimal.h"
#include "CC_Types.generated.h"  // ADD THIS LINE

USTRUCT(BlueprintType)
struct FPickupSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScatterRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeightOffset = 50.f;

	/** XP amount for XP pickups (ignored for other pickup types) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP")
	float XPAmount = 0.f;
};