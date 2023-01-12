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
	const float X = FMath::Clamp(Coords.X, 0, Size.X - 1);
	const float Y = FMath::Clamp(Coords.Y, 0, Size.Y - 1);
	const float Z = FMath::Clamp(Coords.Z, 0, Size.Z - 1);
	Data[X + Y * Size.X + Z * Size.X * Size.Y] = Value;
}

void FGrid3D::Add(const FIntVector Coords, const float Value)
{
	const float X = FMath::Clamp(Coords.X, 0, Size.X - 1);
	const float Y = FMath::Clamp(Coords.Y, 0, Size.Y - 1);
	const float Z = FMath::Clamp(Coords.Z, 0, Size.Z - 1);
	Data[X + Y * Size.X + Z * Size.X * Size.Y] += Value;
}

float FGrid3D::Get(const FIntVector Coords) const
{
	const float X = FMath::Clamp(Coords.X, 0, Size.X - 1);
	const float Y = FMath::Clamp(Coords.Y, 0, Size.Y - 1);
	const float Z = FMath::Clamp(Coords.Z, 0, Size.Z - 1);
	return Data[X + Y * Size.X + Z * Size.X * Size.Y];
}

void FGrid3D::AddInterpolated(const FVector Coords, const float Value)
{
	const float XFrac = FMath::Frac(Coords.X);
	const float YFrac = FMath::Frac(Coords.Y);
	const float ZFrac = FMath::Frac(Coords.Z);
	const int X = FMath::FloorToInt(Coords.X);
	const int Y = FMath::FloorToInt(Coords.Y);
	const int Z = FMath::FloorToInt(Coords.Z);
	
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
	Add(FIntVector(X, Y, Z + 1), Val100);
	Add(FIntVector(X + 1, Y, Z), Val010);
	Add(FIntVector(X + 1, Y, Z + 1), Val110);
	Add(FIntVector(X, Y + 1, Z), Val001);
	Add(FIntVector(X, Y + 1, Z + 1), Val101);
	Add(FIntVector(X + 1, Y + 1, Z), Val011);
	Add(FIntVector(X + 1, Y + 1, Z + 1), Val111);
}

float FGrid3D::GetInterpolated(const FVector Coords) const
{
	const float XFrac = FMath::Frac(Coords.X);
	const float YFrac = FMath::Frac(Coords.Y);
	const float ZFrac = FMath::Frac(Coords.Z);
	const int X = FMath::FloorToInt(Coords.X);
	const int Y = FMath::FloorToInt(Coords.Y);
	const int Z = FMath::FloorToInt(Coords.Z);

	const float Val000 = Get(FIntVector(X, Y, Z));
	const float Val010 = Get(FIntVector(X + 1, Y, Z));
	const float Val001 = Get(FIntVector(X, Y + 1, Z));
	const float Val011 = Get(FIntVector(X + 1, Y + 1, Z));
	const float Val100 = Get(FIntVector(X, Y, Z + 1));
	const float Val110 = Get(FIntVector(X + 1, Y, Z + 1));
	const float Val101 = Get(FIntVector(X, Y + 1, Z + 1));
	const float Val111 = Get(FIntVector(X + 1, Y + 1, Z + 1));

	const float Val00 = (1 - XFrac) * Val000 + XFrac * Val010;
	const float Val01 = (1 - XFrac) * Val001 + XFrac * Val011;
	const float Val10 = (1 - XFrac) * Val100 + XFrac * Val110;
	const float Val11 = (1 - XFrac) * Val101 + XFrac * Val111;

	const float Val0 = (1 - YFrac) * Val00 + YFrac * Val01;
	const float Val1 = (1 - YFrac) * Val10 + YFrac * Val11;
	return (1 - ZFrac) * Val0 + ZFrac * Val1;
}

void FGrid3D::Clear()
{
	for (int i = 0; i < Data.Num(); i++)
	{
		Data[i] = 0;
	}
}
