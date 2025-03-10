// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

UCLASS(BlueprintType)
class SLIMEMOLD_API USkeletonPoint : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldPos = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldNormal = FVector::ZAxisVector;
};

USTRUCT(BlueprintType)
struct SLIMEMOLD_API FSkeletonLine
{
	GENERATED_BODY()

	FSkeletonLine() {}
	FSkeletonLine(USkeletonPoint* i, USkeletonPoint* j) : Point1(i), Point2(j) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* Point1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* Point2 = nullptr;

	bool operator==(const FSkeletonLine& Other) const
	{
		return (this->Point1 == Other.Point1 && this->Point2 == Other.Point2)
			|| (this->Point1 == Other.Point2 && this->Point2 == Other.Point1);
	}
}; 