// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "Components/DynamicMeshComponent.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"


bool UMyBlueprintFunctionLibrary::EnableVertexColors(UDynamicMeshComponent* MeshComponent)
{
	check(MeshComponent);
	using namespace UE::Geometry;

	FDynamicMesh3& Mesh = MeshComponent->GetDynamicMesh()->GetMeshRef();

	Mesh.EnableVertexColors(FVector3f(0.0f));

	Mesh.EnableAttributes();
	Mesh.Attributes()->EnablePrimaryColors();

	MeshComponent->NotifyMeshUpdated();
	
	return Mesh.HasVertexColors();
}

bool UMyBlueprintFunctionLibrary::UpdateVertexColors(UDynamicMeshComponent* MeshComponent, const FVector3f& NewVertexColor)
{
	check(MeshComponent);
	using namespace UE::Geometry;

	MeshComponent->GetDynamicMesh()->EditMesh([&NewVertexColor](FDynamicMesh3& EditMesh)
	{
		EditMesh.EnableVertexColors(FVector3f(0.0f));

		EditMesh.EnableAttributes();
		EditMesh.Attributes()->EnablePrimaryColors();

		FDynamicMeshColorOverlay* ColorOverlay = EditMesh.Attributes()->PrimaryColors();

		FDynamicMesh3* Mesh = ColorOverlay->GetParentMesh();

		ColorOverlay->ClearElements();

		TArray<int> VertToColorMap;
		VertToColorMap.SetNumUninitialized(Mesh->MaxVertexID());
		for (int vid : Mesh->VertexIndicesItr())
		{
			Mesh->SetVertexColor(vid, NewVertexColor);
			int nid = ColorOverlay->AppendElement(NewVertexColor);
			VertToColorMap[vid] = nid;
		}

		for (int tid : Mesh->TriangleIndicesItr())
		{
			FIndex3i Tri = Mesh->GetTriangle(tid);
			Tri.A = VertToColorMap[Tri.A];
			Tri.B = VertToColorMap[Tri.B];
			Tri.C = VertToColorMap[Tri.C];
			ColorOverlay->SetTriangle(tid, Tri);
		}
	},
		EDynamicMeshChangeType::GeneralEdit,
		EDynamicMeshAttributeChangeFlags::VertexColors,
		true);

	MeshComponent->NotifyMeshUpdated();




	FDynamicMesh3* Mesh = MeshComponent->GetDynamicMesh()->GetMeshPtr();

	bool testV1 = MeshComponent->IsEditable();
	//bool testV2 = MeshComponent->GetDynamicMesh()->HasVertexColors();
	bool testV3 = MeshComponent->GetDynamicMesh()->GetMeshPtr()->HasVertexColors();
	bool testV4 = MeshComponent->GetDynamicMesh()->GetMeshPtr()->HasAttributes();
	FVector3f testV5 = Mesh->GetVertexColor(0);
	FVector3f testV6 = Mesh->GetVertexColor(10);
	FVector3f testV7 = Mesh->GetVertexColor(100);

	UE_LOG(LogTemp, Warning, TEXT("testV1: %s"), *FString::Printf(TEXT("%d"), testV1));
	//UE_LOG(LogTemp, Warning, TEXT("testV2: %s"), *FString::Printf(TEXT("%d"), testV2));
	UE_LOG(LogTemp, Warning, TEXT("testV3: %s"), *FString::Printf(TEXT("%d"), testV3));
	UE_LOG(LogTemp, Warning, TEXT("testV4: %s"), *FString::Printf(TEXT("%d"), testV4));
	UE_LOG(LogTemp, Warning, TEXT("testV5: %s"), *testV5.ToString());
	UE_LOG(LogTemp, Warning, TEXT("testV6: %s"), *testV6.ToString());
	UE_LOG(LogTemp, Warning, TEXT("testV7: %s"), *testV7.ToString());


	return true;
}

void UMyBlueprintFunctionLibrary::InitializeOverlayToPerVertexColors(UE::Geometry::FDynamicMeshColorOverlay* ColorOverlay)
{
	
}




//void UMyBlueprintFunctionLibrary::UpdateVertexNormal(
//	UDynamicMesh* TargetMesh,
//	int VertexID,
//	FVector NewColor,
//	bool& bIsValidVertex,
//	bool bMergeSplitNormals,
//	bool bDeferChangeNotifications)
//{
//
//	using namespace UE::Geometry;
//	if (TargetMesh == nullptr)
//	{
//		UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: TargetMesh is Null"));
//		return;
//	}
//
//	TArray<FIndex3i> TriVtxElements;
//	TArray<int32> UniqueElementIDs;
//	TriVtxElements.Reserve(16);
//	UniqueElementIDs.Reserve(16);
//
//	// Updates VertexID's associated elements in Overlay with NewValue. 
//	// Returns false if there were no elements to update (ie all "unset" triangles)
//	auto UpdateOverlay = [VertexID, &TriVtxElements, &UniqueElementIDs, bMergeSplitNormals](FDynamicMesh3& EditMesh, FDynamicMeshColorOverlay* Overlay, FVector4f NewValue)
//		{
//			TriVtxElements.Reset(); UniqueElementIDs.Reset();
//			EditMesh.EnumerateVertexTriangles(VertexID, [&](int32 TriangleID)
//				{
//					if (Overlay->IsSetTriangle(TriangleID))
//					{
//						FIndex3i Tri = EditMesh.GetTriangle(TriangleID);
//						int32 Index = Tri.IndexOf(VertexID);
//						FIndex3i OverlayTri = Overlay->GetTriangle(TriangleID);
//						TriVtxElements.Add(FIndex3i(TriangleID, Index, OverlayTri[Index]));
//						UniqueElementIDs.AddUnique(OverlayTri[Index]);
//					}
//				});
//			if (TriVtxElements.IsEmpty())
//			{
//				return false;
//			}
//			if (UniqueElementIDs.Num() == 1 || bMergeSplitNormals == false)
//			{
//				// just update existing elements to new normal
//				for (int32 ElementID : UniqueElementIDs)
//				{
//					Overlay->SetElement(ElementID, NewValue);
//				}
//			}
//			else   // bMergeSplitNormals == true && UniqueElementIDs.Num() > 1
//			{
//				Overlay->SetElement(UniqueElementIDs[0], NewValue);
//				for (FIndex3i TriInfo : TriVtxElements)
//				{
//					FIndex3i OverlayTri = Overlay->GetTriangle(TriInfo.A);
//					OverlayTri[TriInfo.B] = UniqueElementIDs[0];
//					Overlay->SetTriangle(TriInfo.A, OverlayTri);
//				}
//			}
//			return true;
//		};
//
//	bIsValidVertex = false;
//	TargetMesh->EditMesh([&](FDynamicMesh3& EditMesh)
//		{
//			if (EditMesh.HasAttributes() == false
//				|| (EditMesh.HasAttributes() && EditMesh.Attributes()->PrimaryNormals() == nullptr)
//				|| (EditMesh.HasAttributes() && EditMesh.Attributes()->PrimaryNormals()->ElementCount() == 0))
//			{
//				UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: TargetMesh does not have valid Normals attributes. Try computing Vertex Normals before using UpdateVertexNormal."));
//				return;
//			}
//			if (bUpdateTangents && EditMesh.Attributes()->HasTangentSpace() == false)
//			{
//				UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: TargetMesh does not have valid Tangents attributes. Try computing Tangents before using UpdateVertexNormal."));
//				return;
//			}
//			if (EditMesh.IsVertex(VertexID) == false)
//			{
//				UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: VertexID %d is not a valid vertex in TargetMesh"), VertexID);
//				return;
//			}
//			bIsValidVertex = true;
//
//			if (bUpdateNormal)
//			{
//				FDynamicMeshNormalOverlay* Normals = EditMesh.Attributes()->PrimaryNormals();
//				if (UpdateOverlay(EditMesh, Normals, NewNormal) == false)
//				{
//					UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: VertexID %d has no existing normals in Normal Overlay. Try computing Vertex Normals before using UpdateVertexNormal."), VertexID);
//					bIsValidVertex = false;
//				}
//			}
//			if (bUpdateTangents)
//			{
//				FDynamicMeshNormalOverlay* TangentX = EditMesh.Attributes()->PrimaryTangents();
//				FDynamicMeshNormalOverlay* TangentY = EditMesh.Attributes()->PrimaryBiTangents();
//				bool bTangentXOK = UpdateOverlay(EditMesh, TangentX, NewTangentX);
//				bool bTangentYOK = UpdateOverlay(EditMesh, TangentY, NewTangentY);
//				if (bTangentXOK == false || bTangentYOK == false)
//				{
//					UE_LOG(LogGeometry, Warning, TEXT("UpdateVertexNormal: VertexID %d has no existing tangents in Tangent Overlay. Try computing Tangents before using UpdateVertexNormal."), VertexID);
//					bIsValidVertex = false;
//				}
//			}
//
//		}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::Unknown, bDeferChangeNotifications);
//
//	return TargetMesh;
//}