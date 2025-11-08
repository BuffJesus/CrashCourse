// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_RewardXP.generated.h"

/**
 * Passive ability that listens for KillScored events and grants XP to the killer
 */
UCLASS()
class CRASHCOURSE_API UCC_RewardXP : public UCC_GameplayAbility
{
	GENERATED_BODY()

public:
	UCC_RewardXP();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnKillScoredEvent(FGameplayEventData Payload);
	
	void GrantXP(AActor* Killer, float XPAmount);
};
