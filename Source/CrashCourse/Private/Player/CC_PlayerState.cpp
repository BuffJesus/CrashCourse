// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CC_PlayerState.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"

ACC_PlayerState::ACC_PlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACC_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
