// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/CC_SpawnDeathLoot.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/CC_Tags.h"
#include "Interfaces/CC_DeathPickupInterface.h"
#include "Utils/CC_Types.h"

UCC_SpawnDeathLoot::UCC_SpawnDeathLoot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	// Use SetAssetTags instead of deprecated AbilityTags
	FGameplayTagContainer Tags;
	Tags.AddTag(CCTags::CCAbilities::ActivateOnGiven.GetTag());
	SetAssetTags(Tags);
}

void UCC_SpawnDeathLoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Wait for KillScored events - the delegate expects a pointer to FGameplayEventData
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

void UCC_SpawnDeathLoot::EndAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// Cleanup event binding
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

void UCC_SpawnDeathLoot::OnKillScoredEvent(FGameplayEventData Payload)
{
	// The Instigator in the Payload is the actor that died
	if (AActor* DeadActor = const_cast<AActor*>(Payload.Instigator.Get()))
	{
		SpawnDeathPickups(DeadActor);
	}
}

void UCC_SpawnDeathLoot::SpawnDeathPickups(AActor* DeadActor)
{
	// Only spawn on server (multiplayer safety)
	if (!DeadActor || !DeadActor->HasAuthority())
	{
		return;
	}
	
	// Check if actor implements the death pickup interface
	if (!DeadActor->Implements<UCC_DeathPickupInterface>())
	{
		return;
	}
	
	UWorld* World = DeadActor->GetWorld();
	if (!World)
	{
		return;
	}
	
	// Get pickup configuration from the dead actor
	const TArray<FPickupSpawnInfo> PickupConfigs = ICC_DeathPickupInterface::Execute_GetDeathPickups(DeadActor);
	
	if (PickupConfigs.Num() == 0)
	{
		return;
	}
	
	const FVector DeathLocation = DeadActor->GetActorLocation();
	const FRotator SpawnRotation = FRotator::ZeroRotator;
	
	// Setup spawn parameters once
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = DeadActor->GetInstigator();
	
	// Spawn each configured pickup type
	for (const FPickupSpawnInfo& PickupInfo : PickupConfigs)
	{
		// Validate pickup class
		if (!PickupInfo.PickupClass)
		{
			continue;
		}
		
		// Roll for spawn probability (early exit if failed)
		if (FMath::FRand() > PickupInfo.SpawnChance)
		{
			continue;
		}
		
		// Determine how many of this pickup to spawn
		const int32 SpawnCount = FMath::RandRange(PickupInfo.MinSpawnCount, PickupInfo.MaxSpawnCount);
		
		// Spawn the pickups
		for (int32 i = 0; i < SpawnCount; ++i)
		{
			// Calculate random spawn location with scatter using spherical distribution
			FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.f, PickupInfo.ScatterRadius);
			RandomOffset.Z = 0.f; // Keep horizontal scatter
			
			FVector SpawnLocation = DeathLocation + RandomOffset;
			
			// Optional: Trace down to find ground for better placement
			FHitResult HitResult;
			const FVector TraceStart = SpawnLocation + FVector(0.f, 0.f, 50.f);
			const FVector TraceEnd = SpawnLocation - FVector(0.f, 0.f, 300.f);
			
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(DeadActor);
			
			if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
			{
				SpawnLocation = HitResult.Location;
			}
			
			// Apply height offset after ground trace
			SpawnLocation.Z += PickupInfo.HeightOffset;
			
			// Spawn the pickup
			AActor* SpawnedPickup = World->SpawnActor<AActor>(
				PickupInfo.PickupClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);
			
			if (SpawnedPickup)
			{
				// Optional: Add upward impulse if pickup has physics enabled
				if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SpawnedPickup->GetRootComponent()))
				{
					if (RootPrimitive->IsSimulatingPhysics())
					{
						// Create random impulse direction (mostly upward with some horizontal variation)
						const FVector HorizontalVariation = FMath::VRand() * FMath::RandRange(0.3f, 0.7f);
						const float VerticalStrength = FMath::RandRange(1.5f, 2.5f);
						const FVector ImpulseDirection = FVector(HorizontalVariation.X, HorizontalVariation.Y, VerticalStrength).GetSafeNormal();
						
						// Apply impulse for nice "pop" effect
						const float ImpulseStrength = 400.f;
						RootPrimitive->AddImpulse(ImpulseDirection * ImpulseStrength, NAME_None, true);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn pickup of class %s at location %s"), 
					*PickupInfo.PickupClass->GetName(), *SpawnLocation.ToString());
			}
		}
	}
}
