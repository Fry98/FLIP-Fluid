// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid3D.h"
#include "Particle.h"
#include "GameFramework/Actor.h"
#include "Spatial/DenseGrid3.h"
#include "FluidSim.generated.h"

UCLASS()
class FLIP_API AFluidSim : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFluidSim();

	UPROPERTY(EditAnywhere)
	FIntVector GridSize = FIntVector(40, 17, 30);

	UPROPERTY(EditAnywhere)
	FIntVector SpawnPosition = FIntVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
	FIntVector SpawnSize = FIntVector(15, 15, 17);

	UPROPERTY(EditAnywhere)
	bool Pour = false;

	UPROPERTY(EditAnywhere)
	float CellSize = 10.f;

	UPROPERTY(EditAnywhere)
	int MaxDensity = 6;

	UPROPERTY(EditAnywhere)
	float Gravity = 8.f;

	UPROPERTY(EditAnywhere)
	int JacobiIters = 40;

	UPROPERTY(EditAnywhere)
	float FlipRatio = .85f;

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
	
	FGrid3D VelXSumGrid;
	FGrid3D VelYSumGrid;
	FGrid3D VelZSumGrid;
	
	FGrid3D WeightXSumGrid;
	FGrid3D WeightYSumGrid;
	FGrid3D WeightZSumGrid;
	FGrid3D WeightScalarSumGrid;

	FGrid3D DivergenceGrid;
	FGrid3D PressureGridFront;
	FGrid3D PressureGridBack;

	FGrid3D VelocityXGridBack;
	FGrid3D VelocityYGridBack;
	FGrid3D VelocityZGridBack;
	FGrid3D VelocityXGridFront;
	FGrid3D VelocityYGridFront;
	FGrid3D VelocityZGridFront;
private:
	void ResizeBoxes() const;
	void EnforceBounds();
	void SpawnInVolume();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
