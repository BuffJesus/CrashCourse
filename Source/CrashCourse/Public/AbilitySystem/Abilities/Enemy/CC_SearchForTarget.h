// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_SearchForTarget.generated.h"

class UCC_AITask_FindAndMoveToTarget;
class ACC_BaseCharacter;
class UAbilityTask_WaitDelay;
class UAbilityTask_WaitGameplayEvent;
class AAIController;
class ACC_EnemyCharacter;

/**
 * Enemy ability that continuously searches for and attacks player targets
 */
UCLASS()
class CRASHCOURSE_API UCC_SearchForTarget : public UCC_GameplayAbility
{
	GENERATED_BODY()

public:

	UCC_SearchForTarget();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	TWeakObjectPtr<ACC_EnemyCharacter> OwningEnemy;
	TWeakObjectPtr<AAIController> OwningAIController;
	TWeakObjectPtr<ACC_BaseCharacter> TargetBaseCharacter;

private:

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitGameplayEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> AttackDelayTask;

	UPROPERTY()
	TObjectPtr<UCC_AITask_FindAndMoveToTarget> FindAndMoveTask;

	void StartSearchAndAttackCycle();

	UFUNCTION()
	void OnAttackEnded(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetReached(AActor* Target);

	UFUNCTION()
	void OnNoTargetFound();

	UFUNCTION()
	void ExecuteAttack();
};