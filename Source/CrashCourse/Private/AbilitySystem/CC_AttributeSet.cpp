// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CC_AttributeSet.h"
#include "Interfaces/CC_DeathPickupInterface.h"
#include "Utils/CC_Types.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CC_Tags.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UCC_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(ThisClass, bAttributesInitialized);
}

void UCC_AttributeSet::OnRep_AttributesInitialized()
{
	if (bAttributesInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UCC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
    
	// Only clamp if MaxHealth has been initialized
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && GetMaxHealth() > 0.f)
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
    
	// Only clamp if MaxMana has been initialized
	if (Data.EvaluatedData.Attribute == GetManaAttribute() && GetMaxMana() > 0.f)
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
    
	// Handle death event AND spawn pickups
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && GetHealth() <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health reached zero, spawning pickups"));

		// Send kill scored event
		FGameplayEventData Payload;
		Payload.Instigator = Data.Target.GetAvatarActor();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			Data.EffectSpec.GetEffectContext().GetInstigator(), 
			CCTags::Events::KillScored.GetTag(), 
			Payload
		);
	}

	// Broadcast initialization
	if (!bAttributesInitialized)
	{
		bAttributesInitialized = true;
		OnAttributesInitialized.Broadcast();
	}
}

void UCC_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
    
	// Always clamp to a minimum of 0
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UCC_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UCC_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UCC_AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UCC_AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}
