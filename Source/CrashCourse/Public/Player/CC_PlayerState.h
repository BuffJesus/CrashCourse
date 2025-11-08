// CC_PlayerState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CC_PlayerState.generated.h"

class UCC_AbilitySystemComponent;
class UCC_AttributeSet;

UCLASS()
class CRASHCOURSE_API ACC_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACC_PlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// ADD THIS FUNCTION
	UCC_AttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "XP")
	TObjectPtr<UCurveTable> XPCurveTable;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCC_AttributeSet> AttributeSet;
};