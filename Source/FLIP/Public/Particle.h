// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particle.generated.h"

class AFluidSim;

UCLASS()
class FLIP_API AParticle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticle();

	UFUNCTION()
	void SetSimulation(AFluidSim* Sim);

	UFUNCTION()
	void SetParticlePosition(const FVector Pos);

	UFUNCTION()
	FVector GetParticlePosition() const;

	UFUNCTION()
	void SetParticleVelocity(const FVector Vel);

	UFUNCTION()
	FVector GetParticleVelocity() const;
	
	UPROPERTY(EditAnywhere)
    class UMaterial* WaterMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UStaticMeshComponent* Sphere;

	UPROPERTY()
	AFluidSim* Simulation;

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector Velocity = FVector(0.0);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
