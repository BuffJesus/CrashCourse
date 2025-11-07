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
		
		// Spawn pickups
		if (AActor* OwnerActor = GetOwningActor())
		{
			SpawnDeathPickups(OwnerActor);
		}
		
		// Send kill scored event
		FGameplayEventData Payload;
		Payload.Instigator = Data.Target.GetAvatarActor();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			Data.EffectSpec.GetEffectContext().GetInstigator(), 
			CCTags::Events::KillScored, 
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

void UCC_AttributeSet::SpawnDeathPickups(AActor* DeadActor)
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnDeathPickups called for: %s"), *DeadActor->GetName());
    
    // Only spawn on server (multiplayer safety)
    if (!DeadActor || !DeadActor->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed authority check - HasAuthority: %s"), DeadActor ? (DeadActor->HasAuthority() ? TEXT("true") : TEXT("false")) : TEXT("null"));
        return;
    }
    
    // Check if actor implements the death pickup interface
    if (!DeadActor->Implements<UCC_DeathPickupInterface>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor does not implement ICC_DeathPickupInterface"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Interface check passed!"));
    
    UWorld* World = DeadActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("No world found"));
        return;
    }
    
    // Get pickup configuration from the dead actor
    TArray<FPickupSpawnInfo> PickupConfigs = ICC_DeathPickupInterface::Execute_GetDeathPickups(DeadActor);
    
    UE_LOG(LogTemp, Warning, TEXT("Pickup config count: %d"), PickupConfigs.Num());
    
    FVector DeathLocation = DeadActor->GetActorLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;
    
    // Spawn each configured pickup type
    for (int32 ConfigIndex = 0; ConfigIndex < PickupConfigs.Num(); ++ConfigIndex)
    {
        const FPickupSpawnInfo& PickupInfo = PickupConfigs[ConfigIndex];
        
        UE_LOG(LogTemp, Warning, TEXT("Processing pickup %d - Class: %s"), ConfigIndex, PickupInfo.PickupClass ? *PickupInfo.PickupClass->GetName() : TEXT("NULL"));
        
        // Validate pickup class
        if (!PickupInfo.PickupClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Pickup class is null, skipping"));
            continue;
        }
        
        // Roll for spawn probability
        float RandomRoll = FMath::FRand();
        UE_LOG(LogTemp, Warning, TEXT("Spawn chance: %f, Random roll: %f"), PickupInfo.SpawnChance, RandomRoll);
        
        if (RandomRoll > PickupInfo.SpawnChance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed probability check, skipping"));
            continue; // Failed the probability check
        }
        
        // Determine how many of this pickup to spawn
        int32 SpawnCount = FMath::RandRange(PickupInfo.MinSpawnCount, PickupInfo.MaxSpawnCount);
        UE_LOG(LogTemp, Warning, TEXT("Spawning %d pickups"), SpawnCount);
        
        // Spawn the pickups
        for (int32 i = 0; i < SpawnCount; ++i)
        {
            // Calculate random spawn location with scatter
            FVector RandomOffset = FVector(
                FMath::RandRange(-PickupInfo.ScatterRadius, PickupInfo.ScatterRadius),
                FMath::RandRange(-PickupInfo.ScatterRadius, PickupInfo.ScatterRadius),
                PickupInfo.HeightOffset
            );
            
            FVector SpawnLocation = DeathLocation + RandomOffset;
            
            UE_LOG(LogTemp, Warning, TEXT("Attempting spawn at location: %s"), *SpawnLocation.ToString());
            
            // Setup spawn parameters
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            
            // Spawn the pickup
            AActor* SpawnedPickup = World->SpawnActor<AActor>(
                PickupInfo.PickupClass,
                SpawnLocation,
                SpawnRotation,
                SpawnParams
            );
            
            if (SpawnedPickup)
            {
                UE_LOG(LogTemp, Warning, TEXT("Successfully spawned pickup: %s"), *SpawnedPickup->GetName());
                
                // Optional: Add upward impulse if pickup has physics enabled
                if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SpawnedPickup->GetRootComponent()))
                {
                    if (RootPrimitive->IsSimulatingPhysics())
                    {
                        // Create random impulse direction (mostly upward)
                        FVector ImpulseDirection = FVector(
                            FMath::RandRange(-1.f, 1.f),
                            FMath::RandRange(-1.f, 1.f),
                            FMath::RandRange(1.5f, 2.5f)
                        ).GetSafeNormal();
                        
                        // Apply impulse for nice "pop" effect
                        RootPrimitive->AddImpulse(ImpulseDirection * 400.f, NAME_None, true);
                        UE_LOG(LogTemp, Warning, TEXT("Applied physics impulse"));
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn pickup!"));
            }
        }
    }
}
