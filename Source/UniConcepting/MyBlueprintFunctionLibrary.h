// Fill out your copyright notice in the Description page of Proect Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicMeshComponent.h"
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
	static bool EnableVertexColors(UDynamicMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "SlimeMoldEditorTool")
	static bool UpdateVertexColors(UDynamicMeshComponent* MeshComponent, const FVector3f& NewVertexColor);


	static void InitializeOverlayToPerVertexColors(UE::Geometry::FDynamicMeshColorOverlay* ColorOverlay);

	//UFUNCTION(BlueprintCallable, Category = "SlimeMoldEditorTool")
	//void UpdateVertexNormal(
	//	UDynamicMesh* TargetMesh,
	//	int VertexID,
	//	FVector NewColor,
	//	bool& bIsValidVertex,
	//	bool bMergeSplitNormals,
	//	bool bDeferChangeNotifications);
};
