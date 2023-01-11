// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid3D.h"

FGrid3D::FGrid3D()
{
}

FGrid3D::~FGrid3D()
{
}

void FGrid3D::Resize(const FIntVector NewSize)
{
	this->Size = NewSize;
	Data.Reserve(NewSize.X * NewSize.Y * NewSize.Z);
}

void FGrid3D::Set(const FIntVector Coords, const float Value)
{
	Data[Coords.X + Coords.Y * Size.X + Coords.Z * Size.X * Size.Y] = Value;
}

float FGrid3D::Get(const FIntVector Coords) const
{
	return Data[Coords.X + Coords.Y * Size.X + Coords.Z * Size.X * Size.Y];
}

void FGrid3D::AddInterpolated(const FVector Coords, const float Value)
{
}

float FGrid3D::GetInterpolated(const FVector Coords) const
{
	return 0.f;
}

void FGrid3D::Clear()
{
	for (int i = 0; i < Data.Num(); i++)
	{
		Data[i] = 0;
	}
}
