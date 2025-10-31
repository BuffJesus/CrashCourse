// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "CC_WidgetComponent.generated.h"


struct FGameplayAttribute;
class UAbilitySystemComponent;
class UCC_AttributeSet;
class ACC_BaseCharacter;
class UCC_AbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;
	
private:

    TWeakObjectPtr<ACC_BaseCharacter> CrashCharacter;
    TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
    TWeakObjectPtr<UCC_AttributeSet> AttributeSet;

	void InitAbilitySystemData();
	bool IsASCInitialized();
	void InitializeAttributesDelegate();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UFUNCTION()
	void BindToAttributeChanges();
	
};
