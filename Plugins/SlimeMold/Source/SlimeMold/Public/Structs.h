// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FSkeletonPoint
{
	GENERATED_BODY()

	FSkeletonPoint() : WorldPos(FVector::ZeroVector), WorldNormal(FVector::ZAxisVector) {}
	FSkeletonPoint(FVector NewWorldPos, FVector NewWorldNormal) : WorldPos(NewWorldPos), WorldNormal(NewWorldNormal) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldNormal;
};

USTRUCT(BlueprintType)
struct FSkeletonLine
{
	GENERATED_BODY()

	FSkeletonLine() : PointIndex1(-1), PointIndex2(-1) {}
	FSkeletonLine(int i, int j) : PointIndex1(i), PointIndex2(j) {}


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointIndex1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointIndex2;
};