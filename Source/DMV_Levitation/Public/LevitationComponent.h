// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "LevitationComponent.generated.h"

UENUM()
enum class ELevitationType : uint8
{
	Parasail,
	Jetpack,
	Custom
};

UENUM()
enum class EImpulseMode : uint8
{
	Repeat,
	Cooldown,
	Once
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMV_LEVITATION_API ULevitationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevitationComponent();

	UFUNCTION()
	void GravityCurveUpdateCallback(float value);

	// UFUNCTION()
	void ModifyTimeLine(FRuntimeFloatCurve* Curve);

	UFUNCTION(BlueprintCallable, Category = "Levitation")
	void StartLevitation();
	UFUNCTION(BlueprintCallable, Category = "Levitation")
	void StopLevitation();
	
	UFUNCTION()
	void StopVerticalMoment();

protected:
	virtual void BeginPlay() override;

	void InitComponent();

	UPROPERTY()
	float CurrentLevitationValue;
	UFUNCTION(BlueprintCallable, Category = "Levitation")
	float GetLevitationValue() const { return CurrentLevitationValue; }

	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Levitation")
	TObjectPtr<UTimelineComponent> LevitationTimelineComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Levitation")
	ELevitationType LevitationPreset = ELevitationType::Custom;

	// IMPULSE
	// Vertical impulse
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Impulse")
	float LaunchVerticalImpulse = 0.0f;
	
	// Horizontal impulse
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Impulse")
	float LaunchHorizontalImpulse = 0.0f;

	// Impulse cooldown
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Impulse",
		meta = (EditCondition = "LaunchVerticalImpulse != 0.0f || LaunchHorizontalImpulse != 0.0f"))
	EImpulseMode ImpulseMode = EImpulseMode::Once;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Impulse",
		meta = (EditCondition = "ImpulseMode == EImpulseMode::Cooldown"))
	float ImpulseCooldown = 1.0f;
	UPROPERTY()
	bool bImpulseActive = true;
	FTimerHandle ImpulseCooldownTimerHandle;
	UFUNCTION()
	void ResetImpulseCooldown();
	
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Main")
	float StartLevitationScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Main")
	float TargetLevitationScale;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Main")
	FRuntimeFloatCurve LevitationCurve;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Extra")
	bool bStopMomentum = false;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties|Extra", meta = (EditCondition = "bStopMoment"))
	float StopMomentumDelay = 1.0f;
	
	UPROPERTY()
	float DefaultLevitationScale;
	UPROPERTY()
	float CurrentGravityScale;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};


