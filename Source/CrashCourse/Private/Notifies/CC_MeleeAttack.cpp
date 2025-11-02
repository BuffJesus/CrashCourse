// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/CC_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "Chaos/DebugDrawCommand.h"
#include "Characters/CC_PlayerCharacter.h"
#include "GameplayTags/CC_Tags.h"

void UCC_MeleeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp) || !IsValid(MeshComp->GetOwner())) return;

	TArray<FHitResult> Hits = PerformSphereTrace(MeshComp);
	SendEventsToActors(MeshComp, Hits);
}

TArray<FHitResult> UCC_MeleeAttack::PerformSphereTrace(USkeletalMeshComponent* MeshComp) const
{
	TArray<FHitResult> OutHits;
	
	UWorld* World = GEngine->GetWorldFromContextObject(MeshComp, EGetWorldErrorMode::LogAndReturnNull);
	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName, RTS_World);
	const FVector Start = SocketTransform.GetLocation();
	const FVector End = Start - (SocketTransform.GetRotation().GetForwardVector() * SocketExtensionOffset);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());
	FCollisionResponseParams ResponseParams(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	if (!IsValid(World)) return OutHits;
	
	bool const bHit = World->SweepMultiByChannel(OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereTraceRadius),
		Params,
		ResponseParams);

	if (bDrawDebugs)
	{
		DrawDebugSphereTraceMulti(World,
			Start,
			End,
			SphereTraceRadius,
			EDrawDebugTrace::ForDuration,
			bHit,
			OutHits,
			FColor::Red,
			FColor::Green,
			5.f);
	}
	
	return OutHits;
}

void UCC_MeleeAttack::SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const
{
	for (const FHitResult& Hit : Hits)
	{
		ACC_PlayerCharacter* PlayerCharacter = Cast<ACC_PlayerCharacter>(Hit.GetActor());
		if (!IsValid(PlayerCharacter) || !PlayerCharacter->IsAlive()) continue;
		UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddHitResult(Hit);

		FGameplayEventData Payload;
		Payload.Target = PlayerCharacter;
		Payload.ContextHandle = ContextHandle;
		Payload.Instigator = MeshComp->GetOwner();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), CCTags::Events::Enemy::MeleeTraceHit, Payload);
	}
}

