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
	Data.Init(0, NewSize.X * NewSize.Y * NewSize.Z);
}

void FGrid3D::Set(const FIntVector Coords, const float Value)
{
	Data[Coords.X + Coords.Y * Size.X + Coords.Z * Size.X * Size.Y] = Value;
}

void FGrid3D::Add(const FIntVector Coords, const float Value)
{
	Data[Coords.X + Coords.Y * Size.X + Coords.Z * Size.X * Size.Y] += Value;
}

float FGrid3D::Get(const FIntVector Coords) const
{
	return Data[Coords.X + Coords.Y * Size.X + Coords.Z * Size.X * Size.Y];
}

void FGrid3D::AddInterpolated(const FVector Coords, const float Value)
{
	const float XFrac = FMath::Frac(Coords.X);
	const float YFrac = FMath::Frac(Coords.Y);
	const float ZFrac = FMath::Frac(Coords.Z);

	const int X = FMath::Clamp(FMath::FloorToInt(Coords.X), 0, Size.X - 1);
	const int Y = FMath::Clamp(FMath::FloorToInt(Coords.Y), 0, Size.Y - 1);
	const int Z = FMath::Clamp(FMath::FloorToInt(Coords.Z), 0, Size.Z - 1);
	const int XPlus1 = FMath::Clamp(X + 1, 0, Size.X - 1);
	const int YPlus1 = FMath::Clamp(Y + 1, 0, Size.Y - 1);
	const int ZPlus1 = FMath::Clamp(Z + 1, 0, Size.Z - 1);

	const float Val0 = (1.f - YFrac) * Value;
	const float Val1 = YFrac * Value;

	const float Val00 = (1.f - XFrac) * Val0;
	const float Val10 = XFrac * Val0;
	const float Val01 = (1.f - XFrac) * Val1;
	const float Val11 = XFrac * Val1;

	const float Val000 = (1.f - ZFrac) * Val00;
	const float Val100 = ZFrac * Val00;
	const float Val010 = (1.f - ZFrac) * Val10;
	const float Val110 = ZFrac * Val10;
	const float Val001 = (1.f - ZFrac) * Val01;
	const float Val101 = ZFrac * Val01;
	const float Val011 = (1.f - ZFrac) * Val11;
	const float Val111 = ZFrac * Val11;

	Add(FIntVector(X, Y, Z), Val000);
	Add(FIntVector(X, Y, ZPlus1), Val100);
	Add(FIntVector(XPlus1, Y, Z), Val010);
	Add(FIntVector(XPlus1, Y, ZPlus1), Val110);
	Add(FIntVector(X, YPlus1, Z), Val001);
	Add(FIntVector(X, YPlus1, ZPlus1), Val101);
	Add(FIntVector(XPlus1, YPlus1, Z), Val011);
	Add(FIntVector(XPlus1, YPlus1, ZPlus1), Val111);
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
