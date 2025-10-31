// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "GameplayTags/CC_Tags.h"

void UCC_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	HandleAutoActivatedAbility(AbilitySpec);
	
}

void UCC_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivatedAbility(AbilitySpec);
	}
}

void UCC_AbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UCC_AbilitySystemComponent::AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;
	if (Level <= 0) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level += Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UCC_AbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability->GetAssetTags().HasTagExact(CCTags::CCAbilities::ActivateOnGiven))
	{
		TryActivateAbility(AbilitySpec.Handle);
		return;
	}
}
