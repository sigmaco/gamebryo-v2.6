// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "GroundPlane.h"

//---------------------------------------------------------------------------
GroundPlane::GroundPlane(NiPhysXScene* pkScene, NiPoint3& kNorm, 
    const float fD, const float fRad)
{
    m_spGeometry = PlaneGeometry(kNorm, fD * pkScene->GetScaleFactor(), fRad);
    
    NxPlaneShapeDesc kShapeDesc;
    NiPhysXTypes::NiPoint3ToNxVec3(kNorm, kShapeDesc.normal);
    kShapeDesc.d = fD;
    
    NxActorDesc kActorDesc;
    kActorDesc.name = "GroundPlane";
    kActorDesc.shapes.pushBack(&kShapeDesc);
    
    m_pkScene = pkScene->GetPhysXScene();
    m_pkActor = m_pkScene->createActor(kActorDesc);
}
//---------------------------------------------------------------------------
GroundPlane::~GroundPlane()
{
    if (m_pkActor)
        m_pkScene->releaseActor(*m_pkActor);

    m_spGeometry = 0;
}
//---------------------------------------------------------------------------
NiAVObject* GroundPlane::GetGeometry()
{
    return m_spGeometry;
}
//---------------------------------------------------------------------------
NxActor* GroundPlane::GetActor()
{
    return m_pkActor;
}
//---------------------------------------------------------------------------
NiMeshPtr GroundPlane::PlaneGeometry(const NiPoint3& kNorm, const float fD,
    const float fRad)
{
    // Construct a rotation matrix to take the default plane, one with
    // normal (0,0,1) to the required normal.
    NiPoint3 kBiTangent1 = kNorm.Perpendicular();
    NiPoint3 kBiTangent2 = kNorm.Cross(kBiTangent1);
    NiMatrix3 kRotMat(kBiTangent1, kBiTangent2, kNorm);
    
    // And a translation to take the origin to a distance fD along the normal
    NiPoint3 kTransVec = kNorm * fD;
    
    unsigned short usNumVertices = 33;
    NiPoint3* akVertsInPlane = NiNew NiPoint3[usNumVertices];
    NiPoint3* akVertices = NiNew NiPoint3[usNumVertices];
    NiPoint3* akNormals = NiNew NiPoint3[usNumVertices];
    NiColorA* akColors = NiNew NiColorA[usNumVertices];
    NiUInt16 usNumTriangles = 32;
    NiUInt16* ausTriangles = NiAlloc(NiUInt16, usNumTriangles * 3);

    // Set all the vertices
    float fTheta = 0.0f;
    float fThetaStep = NI_PI * 2.0f / 32.0f;
    for(unsigned short i = 0; i < 32; i++ )
    {
        akVertsInPlane[i].x = fRad * NiCos(fTheta);
        akVertsInPlane[i].y = fRad * NiSin(fTheta);
        akVertsInPlane[i].z = 0.0f;
        akNormals[i].x = 0.0f;
        akNormals[i].y = 0.0f;
        akNormals[i].z = 1.0f;
        akColors[i] = NiColorA(1.0, 1.0, 1.0, 1.0);
        fTheta += fThetaStep;
    }
    akVertsInPlane[32].x = 0.0f;
    akVertsInPlane[32].y = 0.0f;
    akVertsInPlane[32].z = 0.0f;
    akNormals[32].x = 0.0f;
    akNormals[32].y = 0.0f;
    akNormals[32].z = 1.0f;
    akColors[32] = NiColorA(1.0, 1.0, 1.0, 1.0);
    
    // Set all the triangles
    for (unsigned short i = 0; i < 32; i++ )
    {
        ausTriangles[i * 3] = i;
        ausTriangles[i * 3 + 1] = i + 1;
        ausTriangles[i * 3 + 2] = 32;
    }
    ausTriangles[usNumTriangles * 3 - 2] = 0;
    
    NiMatrix3::TransformVertices(kRotMat, kTransVec, 33, akVertsInPlane,
        akVertices);

    NiMeshPtr spPlane = NiNew NiMesh();
    spPlane->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiDataStreamRef* pkPositionStreamRef = 
        spPlane->AddStream(
        NiCommonSemantics::POSITION(), 
        0, 
        NiDataStreamElement::F_FLOAT32_3, 
        usNumVertices, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC | NiDataStream::ACCESS_CPU_READ 
        | NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX, 
        akVertices);
    NI_UNUSED_ARG(pkPositionStreamRef);
    NIASSERT(pkPositionStreamRef);

    NiDataStreamRef* pkNormalStreamRef = 
        spPlane->AddStream(
        NiCommonSemantics::NORMAL(), 
        0, 
        NiDataStreamElement::F_FLOAT32_3, 
        usNumVertices, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC
        | NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX, 
        akNormals);
    NI_UNUSED_ARG(pkNormalStreamRef);
    NIASSERT(pkNormalStreamRef);

   NiDataStreamRef* pkColorStreamRef = 
        spPlane->AddStream(
        NiCommonSemantics::COLOR(),
        0,
        NiDataStreamElement::F_FLOAT32_4,
        usNumVertices,
        NiDataStream::ACCESS_CPU_WRITE_STATIC
            | NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX,
        akColors);
    NI_UNUSED_ARG(pkColorStreamRef);
    NIASSERT(pkColorStreamRef);

    NiDataStreamRef* pkIndexStreamRef = 
        spPlane->AddStream(
        NiCommonSemantics::INDEX(),
        0,
        NiDataStreamElement::F_UINT16_1,
        usNumTriangles * 3,
        NiDataStream::ACCESS_CPU_WRITE_STATIC
            | NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX_INDEX,
        ausTriangles);
    NI_UNUSED_ARG(pkIndexStreamRef);
    NIASSERT(pkIndexStreamRef);

    NiDelete [] akVertices;
    NiDelete [] akNormals;
    NiDelete [] akColors;
    NiDelete[] akVertsInPlane;
    NiFree(ausTriangles);

    NiMaterialPropertyPtr spPlaneMaterial = NiNew NiMaterialProperty();
    spPlane->AttachProperty(spPlaneMaterial);
    NiVertexColorProperty* pkVertexColor = NiNew NiVertexColorProperty;
    pkVertexColor->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertexColor->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    spPlane->AttachProperty(pkVertexColor);

    spPlane->RecomputeBounds();
    spPlane->Update(0.0f);
    spPlane->UpdateProperties();
    spPlane->UpdateEffects();
    
    return spPlane;
}
//---------------------------------------------------------------------------
