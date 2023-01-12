// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FLIP_API FGrid3D
{
protected:
	TArray<float> Data;
	FIntVector Size = FIntVector(0);
	
public:
	FGrid3D();
	~FGrid3D();

	void Resize(const FIntVector NewSize);

	void Set(const FIntVector Coords, const float Value);
	void Add(const FIntVector Coords, const float Value);
	float Get(const FIntVector Coords) const;

	void AddInterpolated(const FVector Coords, const float Value);
	float GetInterpolated(const FVector Coords) const;

	void Clear();
};
