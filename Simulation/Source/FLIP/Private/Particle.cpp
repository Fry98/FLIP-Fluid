// Fill out your copyright notice in the Description page of Project Settings.


#include "Particle.h"

#include "FluidSim.h"

// Sets default values
AParticle::AParticle()
{
	PrimaryActorTick.bCanEverTick = false;
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Materials/Water.Water"));
	if (FoundMaterial.Succeeded())
	{
		WaterMaterial = FoundMaterial.Object;
		Sphere->SetMaterial(0, WaterMaterial);
	}
	
	Sphere->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object);
	Sphere->SetRelativeScale3D(FVector(.1f));
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetSimulatePhysics(false);
	Sphere->SetEnableGravity(false);
	Sphere->SetCastShadow(false);

	RootComponent = Sphere;
}

void AParticle::SetSimulation(AFluidSim* Sim)
{
	Simulation = Sim;
}

// Called when the game starts or when spawned
void AParticle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AParticle::SetParticlePosition(const FVector Pos)
{
	Position = Pos;
	SetActorRelativeLocation(FVector(
		-Simulation->GridSize.X * Simulation->CellSize + 2.f * Pos.X,
		Simulation->GridSize.Y * Simulation->CellSize - 2.f * Pos.Y,
		Simulation->GridSize.Z * Simulation->CellSize - 2.f * Pos.Z
	));
}

FVector AParticle::GetParticlePosition() const
{
	return Position;
}

void AParticle::SetParticleVelocity(const FVector Vel)
{
	Velocity = Vel;
}

FVector AParticle::GetParticleVelocity() const
{
	return Velocity;
}

void AParticle::Hide() const
{
	Sphere->SetVisibility(false);
}

// Called every frame
void AParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

