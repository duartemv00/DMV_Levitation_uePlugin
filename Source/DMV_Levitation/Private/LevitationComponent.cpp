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
	// Set movement mode to falling
	CharacterMovementComponent->SetMovementMode(MOVE_Falling);
	// Initial impulse
	CharacterMovementComponent->Velocity += FVector(0, 0, InitialVerticalImpulse);
	// Set gravity scale to start value
	CharacterMovementComponent->GravityScale = StartLevitationScale;
	// Play timeline
	LevitationTimelineComponent->Play();

	if (bStopMoment)
	{
		FTimerHandle StopMomentTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(StopMomentTimerHandle, this, &ULevitationComponent::StopVerticalMoment, StopMomentDelay, false, StopMomentDelay);
	}
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

