// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_SpawnDeathLoot.generated.h"

/**
 * 
 */
UCLASS()
class CRASHCOURSE_API UCC_SpawnDeathLoot : public UCC_GameplayAbility
{
	GENERATED_BODY()

public:
	UCC_SpawnDeathLoot();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
			const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnKillScoredEvent(FGameplayEventData Payload);

	void SpawnDeathPickups(AActor* DeadActor);
};
