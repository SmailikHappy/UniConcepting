// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

UCLASS(BlueprintType)
class USkeletonPoint : public UObject
{
	GENERATED_BODY()

public:

	USkeletonPoint() : WorldPos(FVector::ZeroVector), WorldNormal(FVector::ZAxisVector) {}
	USkeletonPoint(FVector NewWorldPos, FVector NewWorldNormal) : WorldPos(NewWorldPos), WorldNormal(NewWorldNormal) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldNormal;
};

USTRUCT(BlueprintType)
struct FSkeletonLine
{
	GENERATED_BODY()

	FSkeletonLine() {}
	FSkeletonLine(USkeletonPoint* i, USkeletonPoint* j) : Point1(i), Point2(j) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* Point1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* Point2 = nullptr;
};