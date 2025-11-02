// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/CC_BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CC_Tags.h"

void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* HitActor : ActorsHit)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, CCTags::Events::Enemy::HitReact, Payload);
	}
}
