// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CC_RewardXP.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/CC_Tags.h"
#include "AbilitySystem/CC_AttributeSet.h"

UCC_RewardXP::UCC_RewardXP()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FGameplayTagContainer Tags;
	Tags.AddTag(CCTags::CCAbilities::ActivateOnGiven.GetTag());
	SetAssetTags(Tags);
}

void UCC_RewardXP::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayEventMulticastDelegate& EventDelegate = ASC->GenericGameplayEventCallbacks.FindOrAdd(CCTags::Events::KillScored.GetTag());
		EventDelegate.AddLambda([this](const FGameplayEventData* Payload)
		{
			if (Payload)
			{
				OnKillScoredEvent(*Payload);
			}
		});
	}
}

void UCC_RewardXP::EndAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayEventMulticastDelegate* EventDelegate = ASC->GenericGameplayEventCallbacks.Find(CCTags::Events::KillScored.GetTag());
		if (EventDelegate)
		{
			EventDelegate->RemoveAll(this);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCC_RewardXP::OnKillScoredEvent(FGameplayEventData Payload)
{
	// Payload.Instigator is the dead enemy
	AActor* DeadEnemy = const_cast<AActor*>(Payload.Instigator.Get());
	AActor* Killer = GetAvatarActorFromActorInfo();
	
	if (!DeadEnemy || !Killer)
	{
		return;
	}

	// Get XP reward from dead enemy (you'll need to implement this interface/component)
	float XPReward = 50.f; // Default XP
	
	// TODO: Get XP from enemy data via interface
	// if (DeadEnemy->Implements<UCC_XPRewardInterface>())
	// {
	//     XPReward = ICC_XPRewardInterface::Execute_GetXPReward(DeadEnemy);
	// }

	GrantXP(Killer, XPReward);
}

void UCC_RewardXP::GrantXP(AActor* Killer, float XPAmount)
{
	if (!Killer || !Killer->HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Killer);
	if (!ASC)
	{
		return;
	}

	// Create a gameplay effect to add XP
	UGameplayEffect* XPEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("XPReward")));
	XPEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.ModifierMagnitude = FScalableFloat(XPAmount);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UCC_AttributeSet::GetXPAttribute();

	XPEffect->Modifiers.Add(ModifierInfo);

	ASC->ApplyGameplayEffectToSelf(XPEffect, 1.0f, ASC->MakeEffectContext());

	UE_LOG(LogTemp, Log, TEXT("%s gained %f XP"), *Killer->GetName(), XPAmount);
}
