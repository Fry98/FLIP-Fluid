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
}

// Called every frame
void AFluidSim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
