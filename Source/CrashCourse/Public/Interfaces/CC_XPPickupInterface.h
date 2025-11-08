// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CC_XPPickupInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCC_XPPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for pickups that can have their XP amount set dynamically
 */
class CRASHCOURSE_API ICC_XPPickupInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "XP Pickup")
	void SetXPAmount(float Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "XP Pickup")
	float GetXPAmount() const;
};