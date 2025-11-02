// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/CC_SearchForTarget.h"

#include "AIController.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/CC_BaseCharacter.h"
#include "Characters/CC_EnemyCharacter.h"
#include "GameplayTags/CC_Tags.h"
#include "Tasks/CC_AITask_FindAndMoveToTarget.h"

UCC_SearchForTarget::UCC_SearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UCC_SearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OwningEnemy = Cast<ACC_EnemyCharacter>(GetAvatarActorFromActorInfo());
	check(OwningEnemy.IsValid());
	OwningAIController = Cast<AAIController>(OwningEnemy->GetController());
	check(OwningAIController.IsValid());

	StartSearchAndAttackCycle();

	// Listen for attack end to restart the cycle
	WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CCTags::Events::Enemy::EndAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnAttackEnded);
	WaitGameplayEventTask->ReadyForActivation();
}

void UCC_SearchForTarget::StartSearchAndAttackCycle()
{
	if (bDrawDebugs && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Starting search and attack cycle"));
	}
	
	if (!OwningEnemy.IsValid() || !OwningAIController.IsValid()) return;
	
	FindAndMoveTask = UCC_AITask_FindAndMoveToTarget::FindAndMoveToTarget(
		OwningAIController.Get(),
		CrashTags::Player,
		OwningEnemy->AcceptanceRadius,
		OwningEnemy->MinAttackDelay,
		OwningEnemy->MaxAttackDelay
	);
	
	if (FindAndMoveTask)
	{
		FindAndMoveTask->OnTargetReached.AddDynamic(this, &ThisClass::OnTargetReached);
		FindAndMoveTask->OnNoTargetFound.AddDynamic(this, &ThisClass::OnNoTargetFound);
		FindAndMoveTask->ReadyForActivation();
	}
}

void UCC_SearchForTarget::OnTargetReached(AActor* Target)
{
	TargetBaseCharacter = Cast<ACC_BaseCharacter>(Target);
	
	if (!TargetBaseCharacter.IsValid() || !TargetBaseCharacter->IsAlive())
	{
		if (bDrawDebugs && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Target reached but invalid or dead"));
		}
		return; // The task will automatically restart search
	}

	if (bDrawDebugs && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Target reached - preparing to attack"));
	}
	
	// Rotate to face target
	if (OwningEnemy.IsValid())
	{
		OwningEnemy->RotateToTarget(TargetBaseCharacter.Get());
	}
	
	// Wait for rotation animation, then attack
	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimeLineLength());
	AttackDelayTask->OnFinish.AddDynamic(this, &ThisClass::ExecuteAttack);
	AttackDelayTask->ReadyForActivation();
}

void UCC_SearchForTarget::OnNoTargetFound()
{
	if (bDrawDebugs && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("No valid target found - will retry"));
	}
}

void UCC_SearchForTarget::OnAttackEnded(FGameplayEventData Payload)
{
	if (bDrawDebugs && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Attack ended - restarting cycle"));
	}
	
	StartSearchAndAttackCycle();
}

void UCC_SearchForTarget::ExecuteAttack()
{
	if (bDrawDebugs && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Executing attack"));
	}
	
	const FGameplayTag AttackTag = CCTags::CCAbilities::Enemy::Attack;
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer());
}