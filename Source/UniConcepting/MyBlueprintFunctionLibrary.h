// Fill out your copyright notice in the Description page of Proect Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicMeshComponent.h"
//#include "GeometryScriptTypes.h"
//#include "UDinamicMesh.h"
#include <Components/SplineComponent.h>
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UNICONCEPTING_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "SlimeMoldEditorTool")
	static UPARAM(DisplayName = "Target Mesh") UDynamicMesh*
	InitializeVertexColorBuffer(UDynamicMesh* TargetMesh, const FVector3f& DefaultVertexColor);

	UFUNCTION(BlueprintCallable, Category = "SlimeMoldEditorTool")
	static UPARAM(DisplayName = "Target Mesh") UDynamicMesh*
	UpdateVertexColors(UDynamicMesh* TargetMesh, const TArray<int32>& VertexIDs, const FVector3f& NewVertexColor);

	// This function outputs an array of vertices in a radius from the specific location
	UFUNCTION(BlueprintCallable, Category = "SlimeMoldEditorTool")
	static UPARAM(DisplayName = "Target Mesh") UDynamicMesh*
	FindVerticesInRadius(UDynamicMesh* TargetMesh, const FVector3f& Location, float Radius, TArray<int32>& OutVertexIDs);
};
