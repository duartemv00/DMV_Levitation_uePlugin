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
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Levitation")
	TObjectPtr<UTimelineComponent> LevitationTimelineComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Levitation")
	ELevitationType LevitationPreset = ELevitationType::Custom;

	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	float InitialVerticalImpulse = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	float StartLevitationScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	float TargetLevitationScale;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	FRuntimeFloatCurve LevitationCurve;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	bool bStopMoment = false;
	UPROPERTY(EditAnywhere, Category = "Levitation|Properties")
	float StopMomentDelay = 1.0f;
	
	UPROPERTY()
	float DefaultLevitationScale;
	UPROPERTY()
	float CurrentGravityScale;
};
