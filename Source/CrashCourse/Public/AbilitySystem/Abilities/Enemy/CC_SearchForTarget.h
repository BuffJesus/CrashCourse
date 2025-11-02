// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_SearchForTarget.generated.h"

namespace EPathFollowingResult
{
	enum Type : int;
}

class UAITask_MoveTo;
class ACC_BaseCharacter;
class UAbilityTask_WaitDelay;
class UAbilityTask_WaitGameplayEvent;
class AAIController;
class ACC_EnemyCharacter;
/**
 * 
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
	TObjectPtr<UAbilityTask_WaitDelay> SearchDelayTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> AttackDelayTask;

	UPROPERTY()
	TObjectPtr<UAITask_MoveTo> MoveToActorLocationTask;

	void StartSearch();

	UFUNCTION()
	void EndAttackEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void Search();

	UFUNCTION()
	void MoveToTargetAndAttack();

	UFUNCTION()
	void AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);

	UFUNCTION()
	void Attack();
};