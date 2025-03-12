#include "SlimeMoldRuntimeFunctionLibrary.h"

void USlimeMoldRuntimeBlueprintFunctionLibrary::GetPointValues(const FSkeletonLine& FSkeletonLine, FVector& Point1Position, FVector& Point2Position)
{
	Point1Position = FSkeletonLine.Point1->WorldPos;
	Point2Position = FSkeletonLine.Point2->WorldPos;
}