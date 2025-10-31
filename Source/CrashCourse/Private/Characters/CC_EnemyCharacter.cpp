// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CC_EnemyCharacter.h"

#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"


ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ACC_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ACC_EnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}


void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	if (!HasAuthority()) return;

	GiveStartupAbilities();
	InitializeAttributes();

	UCC_AttributeSet* CCAttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CCAttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CCAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

