// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particle.h"
#include "GameFramework/Actor.h"
#include "FluidSim.generated.h"

UCLASS()
class FLIP_API AFluidSim : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFluidSim();

	UPROPERTY(EditAnywhere)
	FIntVector GridSize = FIntVector(50, 30, 20);

	UPROPERTY(EditAnywhere)
	FIntVector SpawnPosition = FIntVector(0, 0, 0);

	UPROPERTY(EditAnywhere)
	FIntVector SpawnSize = FIntVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
	float CellSize = 10.f;

	UPROPERTY(EditAnywhere)
	int Density = 5;

	UPROPERTY(EditAnywhere)
	float FlipRatio = 0.f;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	class UBoxComponent* BoundingBox;

	UPROPERTY()
	UBoxComponent* SpawnBox;

	UPROPERTY()
	TArray<AParticle*> Particles;

private:
	void ResizeBoxes() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
