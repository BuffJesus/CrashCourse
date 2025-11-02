// CC_AITask_FindAndMoveToTarget.cpp

#include "Tasks/CC_AITask_FindAndMoveToTarget.h"

#include "AIController.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/CC_BlueprintLibrary.h"
#include "Characters/CC_BaseCharacter.h"
#include "TimerManager.h"

UCC_AITask_FindAndMoveToTarget* UCC_AITask_FindAndMoveToTarget::FindAndMoveToTarget(
    AAIController* Controller,
    FName TargetTag,
    float AcceptanceRadius,
    float MinSearchDelay,
    float MaxSearchDelay)
{
    UCC_AITask_FindAndMoveToTarget* Task = NewAITask<UCC_AITask_FindAndMoveToTarget>(*Controller);
    
    if (Task)
    {
        Task->TargetActorTag = TargetTag;
        Task->MovementAcceptanceRadius = AcceptanceRadius;
        Task->MinSearchDelayTime = MinSearchDelay;
        Task->MaxSearchDelayTime = MaxSearchDelay;
    }
    
    return Task;
}

void UCC_AITask_FindAndMoveToTarget::Activate()
{
    Super::Activate();
    StartSearchCycle();
}

void UCC_AITask_FindAndMoveToTarget::OnDestroy(bool bInOwnerFinished)
{
    if (MoveTask)
    {
        MoveTask->ExternalCancel();
        MoveTask = nullptr;
    }
    
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SearchTimerHandle);
    }
    
    Super::OnDestroy(bInOwnerFinished);
}

void UCC_AITask_FindAndMoveToTarget::StartSearchCycle()
{
    if (UWorld* World = GetWorld())
    {
        const float SearchDelay = FMath::RandRange(MinSearchDelayTime, MaxSearchDelayTime);
        World->GetTimerManager().SetTimer(
            SearchTimerHandle,
            this,
            &UCC_AITask_FindAndMoveToTarget::ExecuteSearch,
            SearchDelay,
            false
        );
    }
}

void UCC_AITask_FindAndMoveToTarget::ExecuteSearch()
{
    AAIController* AIController = Cast<AAIController>(OwnerController);
    if (!AIController || !AIController->GetPawn())
    {
        OnNoTargetFound.Broadcast();
        StartSearchCycle();
        return;
    }
    
    const FVector SearchOrigin = AIController->GetPawn()->GetActorLocation();
    FClosestActorWithTagResult Result = UCC_BlueprintLibrary::FindClosestActorWithTag(
        this,
        SearchOrigin,
        TargetActorTag
    );
    
    if (!Result.Actor.IsValid())
    {
        OnNoTargetFound.Broadcast();
        StartSearchCycle();
        return;
    }
    
    // Check if target is alive (if it's a character)
    if (ACC_BaseCharacter* TargetCharacter = Cast<ACC_BaseCharacter>(Result.Actor.Get()))
    {
        if (!TargetCharacter->IsAlive())
        {
            OnNoTargetFound.Broadcast();
            StartSearchCycle();
            return;
        }
    }
    
    MoveToTarget(Result.Actor.Get());
}

void UCC_AITask_FindAndMoveToTarget::MoveToTarget(AActor* Target)
{
    AAIController* AIController = Cast<AAIController>(OwnerController);
    if (!AIController)
    {
        EndTask();
        return;
    }
    
    CurrentTarget = Target;
    
    MoveTask = UAITask_MoveTo::AIMoveTo(
        AIController,
        FVector::ZeroVector,
        Target,
        MovementAcceptanceRadius
    );
    
    if (MoveTask)
    {
        MoveTask->OnMoveTaskFinished.AddUObject(this, &UCC_AITask_FindAndMoveToTarget::OnMoveCompleted);
        MoveTask->ReadyForActivation();
    }
    else
    {
        StartSearchCycle();
    }
}

void UCC_AITask_FindAndMoveToTarget::OnMoveCompleted(
    TEnumAsByte<EPathFollowingResult::Type> Result,
    AAIController* AIController)
{
    MoveTask = nullptr;
    
    if (Result == EPathFollowingResult::Success && CurrentTarget.IsValid())
    {
        // Successfully reached target
        OnTargetReached.Broadcast(CurrentTarget.Get());
    }
    else
    {
        // Movement failed or target became invalid
        OnNoTargetFound.Broadcast();
        StartSearchCycle();
    }
    
    CurrentTarget.Reset();
}