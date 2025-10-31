// Fill out your copyright notice in the Description page of Project Settings.


#include "CrashCourse/Public/Player/CC_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameplayTags/CC_Tags.h"
#include "Logging/StructuredLog.h"

void ACC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* Context : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ThisClass::Primary);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::Secondary);
	EnhancedInputComponent->BindAction(TertiaryAction, ETriggerEvent::Started, this, &ThisClass::Tertiary);
}

void ACC_PlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	GetCharacter()->Jump();
}

void ACC_PlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	GetCharacter()->StopJumping();
}

void ACC_PlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Find forward direction
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void ACC_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ACC_PlayerController::Primary()
{
	ActivateAbility(CCTags::CCAbilities::Primary);
	//FGameplayTag PrimaryTag = FGameplayTag::RequestGameplayTag("CCTags.CCAbilities.Primary")
}

void ACC_PlayerController::Secondary()
{
	ActivateAbility(CCTags::CCAbilities::Secondary);
}

void ACC_PlayerController::Tertiary()
{
	ActivateAbility(CCTags::CCAbilities::Tertiary);
}

void ACC_PlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}	
