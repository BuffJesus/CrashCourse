// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"
#include "CC_AITask_FindAndMoveToTarget.generated.h"

namespace EPathFollowingResult
{
	enum Type : int;
}

class UAITask_MoveTo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetReached, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSearchCycleComplete);

UCLASS()
class CRASHCOURSE_API UCC_AITask_FindAndMoveToTarget : public UAITask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTargetReached OnTargetReached;
    
	UPROPERTY(BlueprintAssignable)
	FOnSearchCycleComplete OnNoTargetFound;

	/** 
	 * Creates a task that repeatedly searches for actors with a tag and moves to them
	 * @param Controller - The AI controller to use for movement
	 * @param TargetTag - The tag to search for on actors
	 * @param AcceptanceRadius - How close to get to the target
	 * @param MinSearchDelay - Minimum time between searches
	 * @param MaxSearchDelay - Maximum time between searches
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "Controller"))
	static UCC_AITask_FindAndMoveToTarget* FindAndMoveToTarget(
		AAIController* Controller,
		FName TargetTag,
		float AcceptanceRadius = 100.f,
		float MinSearchDelay = 0.5f,
		float MaxSearchDelay = 2.0f
	);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	FName TargetActorTag;
	float MovementAcceptanceRadius;
	float MinSearchDelayTime;
	float MaxSearchDelayTime;
    
	FTimerHandle SearchTimerHandle;
    
	UPROPERTY()
	TObjectPtr<UAITask_MoveTo> MoveTask;
    
	TWeakObjectPtr<AActor> CurrentTarget;
    
	void StartSearchCycle();
	void ExecuteSearch();
	void MoveToTarget(AActor* Target);
    
	UFUNCTION()
	void OnMoveCompleted(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
};
