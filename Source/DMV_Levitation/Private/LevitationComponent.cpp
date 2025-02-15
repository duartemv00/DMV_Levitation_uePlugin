// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/LevitationComponent.h"

#include "GameFramework/Character.h"


ULevitationComponent::ULevitationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LevitationTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("LevitationTimeline"));
}

void ULevitationComponent::GravityCurveUpdateCallback(float value)
{
	float CurveValue = FMath::Clamp(value, -1.0f, 1.0f);
	// Update gravity scale
	CurrentGravityScale = StartLevitationScale + ((TargetLevitationScale - StartLevitationScale) * CurveValue);
	CharacterMovementComponent->GravityScale = CurrentGravityScale;
}

void ULevitationComponent::ModifyTimeLine(FRuntimeFloatCurve* Curve)
{
	FOnTimelineFloat GravityUpdateHandler;
	GravityUpdateHandler.BindDynamic(this, &ULevitationComponent::GravityCurveUpdateCallback);
	UCurveFloat* TurnCurvePtr = NewObject<UCurveFloat>();
	TurnCurvePtr->FloatCurve = *Curve->GetRichCurve();
	
	LevitationTimelineComponent->AddInterpFloat(Cast<UCurveFloat>(TurnCurvePtr), GravityUpdateHandler, FName("GravityScale"), FName("Gravity"));
	LevitationTimelineComponent->ReregisterComponent();
}

void ULevitationComponent::StartLevitation()
{
	ModifyTimeLine(&LevitationCurve);
	// Initial impulse
	switch (ImpulseMode)
	{
		case EImpulseMode::Repeat:
			CharacterMovementComponent->Velocity += FVector(0, 0, LaunchVerticalImpulse);
			CharacterMovementComponent->Velocity += OwnerCharacter->GetActorForwardVector() *
				FVector(LaunchHorizontalImpulse, LaunchHorizontalImpulse, 0);
			break;
		case EImpulseMode::Cooldown:
			if (bImpulseActive)
			{
				CharacterMovementComponent->Velocity += FVector(0, 0, LaunchVerticalImpulse);
				CharacterMovementComponent->Velocity += OwnerCharacter->GetActorForwardVector() *
					FVector(LaunchHorizontalImpulse, LaunchHorizontalImpulse, 0);
				bImpulseActive = false;
				GetWorld()->GetTimerManager().SetTimer(ImpulseCooldownTimerHandle, this,
					&ULevitationComponent::ResetImpulseCooldown, ImpulseCooldown, false, ImpulseCooldown);
			}
			break;
		case EImpulseMode::Once:
			if (CharacterMovementComponent->CanAttemptJump())
			{
				CharacterMovementComponent->Velocity += FVector(0, 0, LaunchVerticalImpulse);
				CharacterMovementComponent->Velocity += OwnerCharacter->GetActorForwardVector() *
					FVector(LaunchHorizontalImpulse, LaunchHorizontalImpulse, 0);
			}
			break;
	}
	// Set movement mode to falling
	CharacterMovementComponent->SetMovementMode(MOVE_Falling);
	// Set gravity scale to start value
	CharacterMovementComponent->GravityScale = StartLevitationScale;
	// Play timeline
	LevitationTimelineComponent->PlayFromStart();
	if (bStopMomentum)
	{
		FTimerHandle StopMomentTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(StopMomentTimerHandle, this, &ULevitationComponent::StopVerticalMoment, StopMomentumDelay, false, StopMomentumDelay);
	}
}

void ULevitationComponent::ResetImpulseCooldown()
{
	bImpulseActive = true;
}

void ULevitationComponent::StopLevitation()
{
	// Set movement mode to walking
	CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	// Set gravity scale to default value
	CharacterMovementComponent->GravityScale = DefaultLevitationScale;
	// Stop timeline
	LevitationTimelineComponent->Stop();
}

void ULevitationComponent::StopVerticalMoment()
{
	CharacterMovementComponent->Velocity = FVector(CharacterMovementComponent->Velocity.X, CharacterMovementComponent->Velocity.Y, 0);
}


void ULevitationComponent::BeginPlay()
{
	Super::BeginPlay();
	InitComponent();
}

void ULevitationComponent::InitComponent()
{
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		if (CharacterMovementComponent){
			DefaultLevitationScale =  CharacterMovementComponent->GravityScale;
			CurrentGravityScale = DefaultLevitationScale; // Solo al init

			// Set real start and target levitation scale
			StartLevitationScale = DefaultLevitationScale - (abs(DefaultLevitationScale) * StartLevitationScale);
			TargetLevitationScale = DefaultLevitationScale - (abs(DefaultLevitationScale) * TargetLevitationScale);
		}
	}
	
}

void ULevitationComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr)
	{
		FName PropertyName = PropertyChangedEvent.Property->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ULevitationComponent, LevitationPreset))
		{
			if (LevitationPreset == ELevitationType::Jetpack)
			{
				LaunchVerticalImpulse = 250.0f;
				StartLevitationScale = 0.f;
				TargetLevitationScale = 2.5f;
				bStopMomentum = false;
				StopMomentumDelay = 0.f;
			}
			if (LevitationPreset == ELevitationType::Parasail)
			{
				LaunchVerticalImpulse = 250.0f;
				StartLevitationScale = .0f;
				TargetLevitationScale = .7f;
				bStopMomentum = false;
				StopMomentumDelay = .0f;
			}
		}
	}
}

