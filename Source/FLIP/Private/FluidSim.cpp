// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidSim.h"

#include "Particle.h"
#include "Components/BoxComponent.h"

// Sets default values
AFluidSim::AFluidSim()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	BoundingBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->AttachToComponent(BoundingBox, FAttachmentTransformRules::KeepRelativeTransform);
	
	ResizeBoxes();
}

void AFluidSim::ResizeBoxes() const
{
	const FVector BoundExtent = FVector(GridSize) * CellSize;
	BoundingBox->SetBoxExtent(FVector(GridSize) * CellSize);

	const FVector SpawnExtent = FVector(SpawnSize) * CellSize;
	SpawnBox->SetBoxExtent(SpawnExtent);
	SpawnBox->SetRelativeLocation(FVector(
		-BoundExtent.X + SpawnExtent.X + CellSize * 2.f * SpawnPosition.X,
		BoundExtent.Y - SpawnExtent.Y - CellSize * 2.f * SpawnPosition.Y,
		BoundExtent.Z - SpawnExtent.Z - CellSize * 2.f * SpawnPosition.Z
	));
}

void AFluidSim::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ResizeBoxes();
}

// Called when the game starts or when spawned
void AFluidSim::BeginPlay()
{
	Super::BeginPlay();

	for (int z = SpawnPosition.Z; z < SpawnPosition.Z + SpawnSize.Z; z++)
	{
		for (int y = SpawnPosition.Y; y < SpawnPosition.Y + SpawnSize.Y; y++)
		{
			for (int x = SpawnPosition.X; x < SpawnPosition.X + SpawnSize.X; x++)
			{
				for (int i = 0; i < Density; i++)
				{
					FVector Wiggle = FVector(
						FMath::RandRange(0.f, 0.9999f),
						FMath::RandRange(0.f, 0.9999f),
						FMath::RandRange(0.f, 0.9999f)
					);
					
					const auto Particle = GetWorld()->SpawnActor<AParticle>();
					Particle->SetSimulation(this);
					Particle->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
					Particle->SetParticlePosition(FVector(x * CellSize, y * CellSize, z * CellSize) + Wiggle * CellSize);
					Particles.Add(Particle);
				}
			}
		}
	}

	VelXSumGrid.Resize(GridSize + FIntVector(1));
	VelYSumGrid.Resize(GridSize + FIntVector(1));
	VelZSumGrid.Resize(GridSize + FIntVector(1));
	
	WeightXSumGrid.Resize(GridSize + FIntVector(1));
	WeightYSumGrid.Resize(GridSize + FIntVector(1));
	WeightZSumGrid.Resize(GridSize + FIntVector(1));
	WeightScalarSumGrid.Resize(GridSize + FIntVector(1));

	DivergenceGrid.Resize(GridSize);
	PressureGridFront.Resize(GridSize);
	PressureGridBack.Resize(GridSize);

	VelocityXGridBack.Resize(GridSize + FIntVector(1));
	VelocityYGridBack.Resize(GridSize + FIntVector(1));
	VelocityZGridBack.Resize(GridSize + FIntVector(1));
	VelocityXGridFront.Resize(GridSize + FIntVector(1));
	VelocityYGridFront.Resize(GridSize + FIntVector(1));
	VelocityZGridFront.Resize(GridSize + FIntVector(1));
}

// Called every frame
void AFluidSim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Transfer velocity to grid
	VelXSumGrid.Clear();
	VelYSumGrid.Clear();
	VelZSumGrid.Clear();
	WeightScalarSumGrid.Clear();
	WeightXSumGrid.Clear();
	WeightYSumGrid.Clear();
	WeightZSumGrid.Clear();

	for (const auto Particle : Particles)
	{
		const FVector GridPos = Particle->GetParticlePosition() / CellSize;
		const FVector PartVel = Particle->GetParticleVelocity();

		VelXSumGrid.AddInterpolated(GridPos + FVector(.5f, 0.f, 0.f), PartVel.X);
		VelYSumGrid.AddInterpolated(GridPos + FVector(0.f, .5f, 0.f), PartVel.Y);
		VelZSumGrid.AddInterpolated(GridPos + FVector(0.f, 0.f, .5f), PartVel.Z);

		WeightXSumGrid.AddInterpolated(GridPos + FVector(.5f, 0.f, 0.f), 1.0);
		WeightYSumGrid.AddInterpolated(GridPos + FVector(0.f, .5f, 0.f), 1.0);
		WeightZSumGrid.AddInterpolated(GridPos + FVector(0.f, 0.f, .5f), 1.0);
		WeightScalarSumGrid.AddInterpolated(GridPos, 1.0);
	}

	for (int z = 0; z <= GridSize.Z; z++)
	{
		for (int y = 0; y <= GridSize.Y; y++)
		{
			for (int x = 0; x <= GridSize.X; x++)
			{
				const FIntVector Pos = FIntVector(x, y, z);
				
				auto Weight = WeightXSumGrid.Get(Pos);
				const auto NewVelX = Weight > 0.f ? VelXSumGrid.Get(Pos) / Weight : 0.f;

				Weight = WeightYSumGrid.Get(Pos);
				const auto NewVelY = Weight > 0.f ? VelYSumGrid.Get(Pos) / Weight : 0.f;

				Weight = WeightZSumGrid.Get(Pos);
				const auto NewVelZ = Weight > 0.f ? VelZSumGrid.Get(Pos) / Weight : 0.f;

				VelocityXGridFront.Set(Pos, NewVelX);
				VelocityXGridBack.Set(Pos, NewVelX);
				VelocityYGridFront.Set(Pos, NewVelY);
				VelocityYGridBack.Set(Pos, NewVelY);
				VelocityZGridFront.Set(Pos, NewVelZ);
				VelocityZGridBack.Set(Pos, NewVelZ);
			}
		}
	}

	// Add forces
	for (int z = 0; z <= GridSize.Z; z++)
	{
		for (int y = 0; y <= GridSize.Y; y++)
		{
			for (int x = 0; x <= GridSize.X; x++)
			{
				const auto Pos = FIntVector(x, y, z);
				VelocityZGridFront.Set(Pos, VelocityZGridFront.Get(Pos) + Gravity);
			}
		}
	}
}
