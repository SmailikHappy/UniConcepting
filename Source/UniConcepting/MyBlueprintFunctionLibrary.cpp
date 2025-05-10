// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
//#include "UDinamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"


UDynamicMesh* UMyBlueprintFunctionLibrary::InitializeVertexColorBuffer(UDynamicMesh* TargetMesh, const FVector3f& DefaultVertexColor)
{
	check(TargetMesh);
	using namespace UE::Geometry;

	int minVID = 9999999, maxVID = -9999999;

	TargetMesh->EditMesh([&DefaultVertexColor, &minVID, &maxVID](FDynamicMesh3& Mesh)
	{
		Mesh.EnableVertexColors(DefaultVertexColor);

		Mesh.EnableAttributes();
		Mesh.Attributes()->EnablePrimaryColors();

		FDynamicMeshColorOverlay* ColorOverlay = Mesh.Attributes()->PrimaryColors();

		ColorOverlay->ClearElements();

		TArray<int> VertToColorMap;
		VertToColorMap.SetNumUninitialized(Mesh.MaxVertexID());
		for (int vid : Mesh.VertexIndicesItr())
		{
			if (vid < minVID)
			{
				minVID = vid;
			}
			if (vid > maxVID)
			{
				maxVID = vid;
			}
			Mesh.SetVertexColor(vid, DefaultVertexColor);
			int nid = ColorOverlay->AppendElement(DefaultVertexColor);
			VertToColorMap[vid] = nid;
		}

		for (int tid : Mesh.TriangleIndicesItr())
		{
			FIndex3i Tri = Mesh.GetTriangle(tid);
			Tri.A = VertToColorMap[Tri.A];
			Tri.B = VertToColorMap[Tri.B];
			Tri.C = VertToColorMap[Tri.C];
			ColorOverlay->SetTriangle(tid, Tri);
		}
	},
	EDynamicMeshChangeType::GeneralEdit,
	EDynamicMeshAttributeChangeFlags::VertexColors,
	true);

	UE_LOG(LogTemp, Warning, TEXT("max vID: %i, min vID: %i"), maxVID, minVID);
	
	return TargetMesh;
}

UDynamicMesh* UMyBlueprintFunctionLibrary::UpdateVertexColors(UDynamicMesh* TargetMesh, const TArray<int32>& VertexIDs, const FVector3f& NewVertexColor)
{
	check(TargetMesh);
	using namespace UE::Geometry;	

	TargetMesh->EditMesh([&NewVertexColor, &VertexIDs](FDynamicMesh3& EditMesh)
	{
		TArray<FIndex3i> TriVtxElements;
		TArray<int32> UniqueElementIDs;
		TriVtxElements.Reserve(16);
		UniqueElementIDs.Reserve(16);
		FDynamicMeshColorOverlay* ColorOverlay = EditMesh.Attributes()->PrimaryColors();

		for (int32 vid : VertexIDs)
		{
			TriVtxElements.Reset(); UniqueElementIDs.Reset();
			EditMesh.EnumerateVertexTriangles(vid, [&](int32 TriangleID)
			{
				if (ColorOverlay->IsSetTriangle(TriangleID))
				{
					FIndex3i Tri = EditMesh.GetTriangle(TriangleID);
					int32 Index = Tri.IndexOf(vid);
					FIndex3i OverlayTri = ColorOverlay->GetTriangle(TriangleID);
					TriVtxElements.Add(FIndex3i(TriangleID, Index, OverlayTri[Index]));
					UniqueElementIDs.AddUnique(OverlayTri[Index]);
				}
			});

			if (TriVtxElements.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("No triangles found with associated vertex id: %i"), vid);
				continue;
			}
			if (UniqueElementIDs.Num() == 1)
			{
				// just update existing elements to new normal
				for (int32 ElementID : UniqueElementIDs)
				{
					ColorOverlay->SetElement(ElementID, NewVertexColor);
				}
			}
			else   // UniqueElementIDs.Num() > 1
			{
				ColorOverlay->SetElement(UniqueElementIDs[0], NewVertexColor);
				for (FIndex3i TriInfo : TriVtxElements)
				{
					FIndex3i OverlayTri = ColorOverlay->GetTriangle(TriInfo.A);
					OverlayTri[TriInfo.B] = UniqueElementIDs[0];
					ColorOverlay->SetTriangle(TriInfo.A, OverlayTri);
				}
			}
			//EditMesh.SetVertexColor(vid, NewVertexColor);
			// We do not need to set the vertex color here??????
		}
	},
	EDynamicMeshChangeType::GeneralEdit,
	EDynamicMeshAttributeChangeFlags::VertexColors,
	true);

	return TargetMesh;
}
