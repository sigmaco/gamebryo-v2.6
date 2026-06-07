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

// Precompiled Header
#include "NiCollisionPCH.h"

#include <NiBoundingVolume.h>
#include <NiColor.h>
#include <NiMaterialProperty.h>
#include <NiMath.h>
#include <NiNode.h>
#include <NiMesh.h>
#include <NiVertexColorProperty.h>
#include <NiWireframeProperty.h>
#include "NiBoxBV.h"
#include "NiCapsuleBV.h"
#include "NiCollisionData.h"
#include "NiSphereBV.h"
#include "NiUnionBV.h"
#include "NiDrawableBV.h"
#include "NiHalfSpaceBV.h"

//---------------------------------------------------------------------------
NiNode* NiDrawableBV::CreateWireframeBV(const NiBoundingVolume* pkBound, 
    const NiColor& kColor, const float fMult, NiAVObject* pkObject)
{
    // bounding volume must exist
    if (!pkBound)
        return 0;

    NiNode* pkRoot = NiNew NiNode;
    NIASSERT(pkRoot);
    NiCollisionData* pkCollisionData = NiNew NiCollisionData(pkRoot);
    NIASSERT(pkCollisionData);
    pkCollisionData->SetCollisionMode(NiCollisionData::NOTEST);
    pkCollisionData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);

    NiMesh* pkMesh;
    switch (pkBound->Type())
    {
        case NiBoundingVolume::BOX_BV:
            pkMesh = CreateFromBox(((NiBoxBV*)pkBound)->GetBox(), fMult);
            pkRoot->AttachChild(pkMesh);
            break;
        case NiBoundingVolume::CAPSULE_BV:
            pkMesh = CreateFromCapsule(
                ((NiCapsuleBV*)pkBound)->GetCapsule(), fMult);
            pkRoot->AttachChild(pkMesh);
            break;
        case NiBoundingVolume::SPHERE_BV:
            pkMesh = CreateFromSphere(
                ((NiSphereBV*)pkBound)->GetSphere(), fMult);
            pkRoot->AttachChild(pkMesh);
            break;
       case NiBoundingVolume::UNION_BV:
           CreateFromUnion((NiUnionBV*)pkBound, pkRoot, fMult);
           break;
        case NiBoundingVolume::HALFSPACE_BV:
            pkMesh = CreateFromHalfSpaceBV((NiHalfSpaceBV*)pkBound, fMult,
                pkObject);
            pkRoot->AttachChild(pkMesh);
            break;
        default:  // New BV type?
            NiDelete pkRoot;
            return 0;
            break;
    }

    NiWireframeProperty* pkWire = NiNew NiWireframeProperty;
    pkWire->SetWireframe(true);
    pkRoot->AttachProperty(pkWire);

    NiVertexColorProperty* pkVC = NiNew NiVertexColorProperty;
    pkVC->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
    pkVC->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pkRoot->AttachProperty(pkVC);

    NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(kColor);
    pkRoot->AttachProperty(pkMat);

    return pkRoot;
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateMeshFromVertexIndexData(NiPoint3* kVerts, 
    unsigned int uiNumVerts, unsigned short* pusIndices, 
    unsigned int uiNumIndices)
{
    NiMesh* pkNewMesh = NiNew NiMesh();
    NiDataStream* pkDataStream;

    pkNewMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    // Positions.
    // The collision detection system requires 32-bit floats for vertex
    // coordinates, but that's the default data type for export from the
    // art packages.
    pkDataStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_FLOAT32_3,
        uiNumVerts, 
        NiDataStream::ACCESS_CPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC | NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX, kVerts);
    NIASSERT(pkDataStream != NULL);

    pkNewMesh->AddStreamRef(pkDataStream, NiCommonSemantics::POSITION());

    // Indices.  
    pkDataStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT16_1,
        uiNumIndices, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC | NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX_INDEX, pusIndices); 
    NIASSERT(pkDataStream != NULL);

    pkNewMesh->AddStreamRef(pkDataStream, NiCommonSemantics::INDEX());
    pkNewMesh->SetSubmeshCount(1, 0);
    pkNewMesh->RecomputeBounds();

    return pkNewMesh;
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateFromHalfSpaceBV(
    const NiHalfSpaceBV* pkHalfSpaceBV, const float fMult, 
    const NiAVObject* pkObject)
{
    NiPoint3 kAxis = pkHalfSpaceBV->GetPlane().GetNormal();
    NiPoint3 kCenter = pkHalfSpaceBV->GetCenter();
    float fExtent = pkObject->GetWorldBound().GetRadius();

    // create two basis vectors that are perpendicular to the normal
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);

    // use fMult to determine the size of the plane representation
    kBasis0 *= fExtent;
    kBasis1 *= fExtent;
    NiPoint3 kNorm0 = kBasis0 * 0.05f;
    NiPoint3 kNorm1 = kBasis1 * 0.05f;
    NiPoint3 kNorm2 = kAxis * fExtent * 0.5f;
    NiPoint3 kOffset = kNorm2 * (1.0f - fMult);

    const unsigned int uiNumVerts = 4;
    const unsigned int uiNumTris = 2;
    NiPoint3 akPoints[uiNumVerts];
    NiUInt16 ausConn[3 * uiNumTris];

    // plane
    akPoints[0] = kCenter + kBasis0 + kBasis1 + kOffset;
    akPoints[1] = kCenter + kBasis0 - kBasis1 + kOffset;
    akPoints[2] = kCenter - kBasis0 - kBasis1 + kOffset;
    akPoints[3] = kCenter - kBasis0 + kBasis1 + kOffset;
    
    // Indices..
    //ausConn[0]  = 0;  ausConn[1]  = 3;  ausConn[2]  = 1;
    //ausConn[3]  = 3;  ausConn[4]  = 2;  ausConn[5]  = 1;
    ausConn[0]  = 0;  ausConn[1]  = 1;  ausConn[2]  = 3;
    ausConn[3]  = 1;  ausConn[4]  = 2;  ausConn[5]  = 3;


    // Create the mesh and data streams.
    //NILOG("Building half space BV wireframe geometry.\n");
    return CreateMeshFromVertexIndexData(&akPoints[0], uiNumVerts, &ausConn[0],
        uiNumTris * 3);
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateFromBox(const NiBox& kBox, const float fMult)
{
    const float afExtent[3] = {
        kBox.m_afExtent[0] * fMult, 
        kBox.m_afExtent[1] * fMult, 
        kBox.m_afExtent[2] * fMult
    };

    const unsigned int uiNumVerts = 8;
    const unsigned int uiNumTris = 12;

    NiPoint3 akPoints[uiNumVerts];
    unsigned short ausConn[3 * uiNumTris];

    akPoints[0] = kBox.m_kCenter
        + kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    akPoints[1] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    akPoints[2] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    akPoints[3] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        - kBox.m_akAxis[2] * afExtent[2];
    akPoints[4] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    akPoints[5] = kBox.m_kCenter 
        + kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    akPoints[6] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        + kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];
    akPoints[7] = kBox.m_kCenter 
        - kBox.m_akAxis[0] * afExtent[0]
        - kBox.m_akAxis[1] * afExtent[1]
        + kBox.m_akAxis[2] * afExtent[2];

    // Indices...
    ausConn[0]  = 3;  ausConn[1]  = 2;  ausConn[2]  = 1;
    ausConn[3]  = 3;  ausConn[4]  = 1;  ausConn[5]  = 0;
    ausConn[6]  = 0;  ausConn[7]  = 1;  ausConn[8]  = 5;
    ausConn[9]  = 0;  ausConn[10] = 5;  ausConn[11] = 4;
    ausConn[12] = 4;  ausConn[13] = 5;  ausConn[14] = 6;
    ausConn[15] = 4;  ausConn[16] = 6;  ausConn[17] = 7;
    ausConn[18] = 7;  ausConn[19] = 6;  ausConn[20] = 2;
    ausConn[21] = 7;  ausConn[22] = 2;  ausConn[23] = 3;
    ausConn[24] = 1;  ausConn[25] = 2;  ausConn[26] = 6;
    ausConn[27] = 1;  ausConn[28] = 6;  ausConn[29] = 5;
    ausConn[30] = 0;  ausConn[31] = 4;  ausConn[32] = 7;
    ausConn[33] = 0;  ausConn[34] = 7;  ausConn[35] = 3;

    //NILOG("Building box BV wireframe geometry.\n");
    return CreateMeshFromVertexIndexData(&akPoints[0], uiNumVerts, &ausConn[0],
        uiNumTris * 3);
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateFromCapsule(const NiCapsule& kCapsule, 
    const float fMult)
{
    // 1.05f makes wireframe visible over actual BV if fMult = 1.0
    float fMultExt = fMult * 1.05f;
    const float fRadius = kCapsule.m_fRadius * fMultExt;
    NiPoint3 kDirection = kCapsule.m_kSegment.m_kDirection;

    float fLength = kDirection.Length();
    if (fLength < 1e-05)
    {
        // Axis doesn't really matter. This is basically a sphere.
        NiSphere kSphere;
        kSphere.m_kCenter = kCapsule.m_kSegment.m_kOrigin;
        kSphere.m_fRadius = fRadius;
        return CreateFromSphere(kSphere, fMult);
    }

    // compute unit-length kCapsule kAxis
    NiPoint3 kAxis = kDirection;
    kAxis.Unitize();

    // compute end points of kCapsule kAxis
    NiPoint3 kEndPt0 = kCapsule.m_kSegment.m_kOrigin
        - (kDirection + fRadius * kAxis) * (fMultExt - 1.0f);
    NiPoint3 kEndPt1 = kCapsule.m_kSegment.m_kOrigin 
        + kCapsule.m_kSegment.m_kDirection * fMultExt;

    // create two basis vectors that are perpendicular to the kCapsule kAxis
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);
    kBasis0 *= fRadius;
    kBasis1 *= fRadius;

    // cross section of kCapsule is 16-sided regular polygon
    const unsigned int uiNumSides = 16;
    const unsigned int uiNumVerts = 4 * uiNumSides + 2;
    const unsigned int uiNumTris = 8 * uiNumSides;
    NiPoint3 akPoints[uiNumVerts];
    unsigned short ausConn[uiNumTris * 3];

    akPoints[uiNumVerts-2] = kEndPt0 - kAxis * fRadius;
    akPoints[uiNumVerts-1] = kEndPt1 + kAxis * fRadius;

    float fCapScale = NiSqrt(0.5f);
    NiPoint3 kCap0 = kEndPt0 - kAxis * (fRadius * fCapScale);
    NiPoint3 kCap1 = kEndPt1 + kAxis * (fRadius * fCapScale);

    float fIncr = 2.0f * NI_PI / (float)uiNumSides;
    float fTheta = 0.0f;
    unsigned int i;
    for (i = 0; i < uiNumSides; i++)
    {
        NiPoint3 kOffset = kBasis0 * NiCos(fTheta) + kBasis1 * NiSin(fTheta);

        akPoints[i] = kCap0 + kOffset * fCapScale;
        akPoints[i + uiNumSides] = kEndPt0 + kOffset;
        akPoints[i + uiNumSides * 2] = kEndPt1 + kOffset;
        akPoints[i + uiNumSides * 3] = kCap1 + kOffset * fCapScale;

        fTheta += fIncr;
    }

    unsigned short* ausConnTmp = &ausConn[0];
    unsigned int j;
    for (i = 0; i < uiNumSides-1; i++)
    {
        // side
        for (j = 0; j < 3; j++)
        {
            *ausConnTmp++ = (unsigned short)(i + uiNumSides * j);
            *ausConnTmp++ = (unsigned short)(i + 1 + uiNumSides * j);
            *ausConnTmp++ = (unsigned short)(i + 1 + uiNumSides * (j + 1));

            *ausConnTmp++ = (unsigned short)(i + uiNumSides * j);
            *ausConnTmp++ = (unsigned short)(i + 1 + uiNumSides * (j + 1));
            *ausConnTmp++ = (unsigned short)(i + uiNumSides * (j + 1));
        }

        // end caps
        *ausConnTmp++ = (unsigned short)(i + 1);
        *ausConnTmp++ = (unsigned short)i;
        *ausConnTmp++ = (unsigned short)(uiNumVerts - 2);

        *ausConnTmp++ = (unsigned short)(i + uiNumSides * 3);
        *ausConnTmp++ = (unsigned short)(i + 1 + uiNumSides * 3);
        *ausConnTmp++ = (unsigned short)(uiNumVerts - 1);
    }

    for (j = 0; j < 3; j++)
    {
        *ausConnTmp++ = (unsigned short)(uiNumSides * (j + 1) - 1);
        *ausConnTmp++ = (unsigned short)(uiNumSides * j);
        *ausConnTmp++ = (unsigned short)(uiNumSides * (j + 1));

        *ausConnTmp++ = (unsigned short)(uiNumSides * (j + 1) - 1);
        *ausConnTmp++ = (unsigned short)(uiNumSides * (j + 1));
        *ausConnTmp++ = (unsigned short)(uiNumSides * (j + 2) - 1);
    }

    *ausConnTmp++ = 0;
    *ausConnTmp++ = uiNumSides - 1;
    *ausConnTmp++ = uiNumVerts - 2;

    *ausConnTmp++ = uiNumSides * 4 - 1;
    *ausConnTmp++ = uiNumSides * 3;
    *ausConnTmp++ = uiNumVerts - 1;

   //NILOG("Building capsule BV wireframe geometry.\n");
   return CreateMeshFromVertexIndexData(&akPoints[0], uiNumVerts, &ausConn[0],
        uiNumTris * 3);
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateFromSphere(const NiSphere& kSphere, 
    const float fMult)
{
    // 1.05f makes wireframe visible over actual BV if fMult = 1.0
    float fMultExt = fMult * 1.05f;

    // create a kSphere by one subdivision of an icosahedron
    const float fGold = 0.5f * (NiSqrt(5.0f) + 1.0f);
    
    NiPoint3 kCenter = kSphere.m_kCenter;

    NiPoint3 akIcoVerts[12] =
    {
        NiPoint3(fGold,  1.0f,  0.0f), 
        NiPoint3(-fGold,  1.0f,  0.0f), 
        NiPoint3(fGold, -1.0f,  0.0f), 
        NiPoint3(-fGold, -1.0f,  0.0f), 
        NiPoint3(1.0f,  0.0f,  fGold), 
        NiPoint3(1.0f,  0.0f, -fGold), 
        NiPoint3(-1.0f,  0.0f,  fGold), 
        NiPoint3(-1.0f,  0.0f, -fGold), 
        NiPoint3(0.0f,  fGold,  1.0f), 
        NiPoint3(0.0f, -fGold,  1.0f), 
        NiPoint3(0.0f,  fGold, -1.0f), 
        NiPoint3(0.0f, -fGold, -1.0f)
    };

    const unsigned short ausIcoPolys[] =
    {
        0, 8, 4, 
        0, 5, 10, 
        2, 4, 9, 
        2, 11, 5, 
        1, 6, 8, 
        1, 10, 7, 
        3, 9, 6, 
        3, 7, 11, 
        0, 10, 8, 
        1, 8, 10, 
        2, 9, 11, 
        3, 11, 9, 
        4, 2, 0, 
        5, 0, 2, 
        6, 1, 3, 
        7, 3, 1, 
        8, 6, 4, 
        9, 4, 6, 
        10, 5, 7, 
        11, 7, 5
    };

    unsigned int uiNumVerts = 12;
    unsigned int uiNumTris = 20;
    unsigned int uiTotalVerts = 72;
    unsigned int uiTotalTris = 80;

    // Create an array to store all of the points and two arrays (one for
    // scratch space) to store the triangle connectivity.
    NiPoint3* pkPoints = NiNew NiPoint3[uiTotalVerts];
    unsigned int uiTotalTris_X_3 = uiTotalTris * 3;
    unsigned short* pusConn = NiAlloc(unsigned short, uiTotalTris_X_3);
    unsigned short* pusConnTmp = NiAlloc(unsigned short, uiTotalTris_X_3);
    unsigned int uiDestSize = uiTotalTris_X_3 * sizeof(unsigned short);

    // starting with an icosahedron, normalize the vertices
    unsigned int i;
    for(i = 0; i < uiNumVerts; i++)
    {
        akIcoVerts[i].Unitize();
        pkPoints[i] = akIcoVerts[i];
    }

    // start with the icosahedron connectivity, subdivide once
    NiMemcpy(pusConn, uiDestSize, ausIcoPolys, uiDestSize);

    // store traversal pointers for the connectivity arrays
    unsigned short* pusSrc = pusConn;
    unsigned short* pusDest = pusConnTmp;

    for (unsigned int t=0; t < uiNumTris; t++)
    {
        // get the indices to the triangle's vertices
        unsigned short v1 = *pusSrc++, v2 = *pusSrc++, v3 = *pusSrc++;

        // compute the three new verts as triangle edge midpoints and
        // normalize the points back onto the kSphere
        pkPoints[uiNumVerts] = pkPoints[v1] + pkPoints[v2];
        pkPoints[uiNumVerts].Unitize();
        pkPoints[uiNumVerts + 1] = pkPoints[v2] + pkPoints[v3];
        pkPoints[uiNumVerts + 1].Unitize();
        pkPoints[uiNumVerts + 2] = pkPoints[v3] + pkPoints[v1];
        pkPoints[uiNumVerts + 2].Unitize();

        // create 4 new triangles to retessellate the old triangle
        *pusDest++ = v1;
        *pusDest++ = (unsigned short)uiNumVerts;
        *pusDest++ = (unsigned short)(uiNumVerts + 2);

        *pusDest++ = v2;
        *pusDest++ = (unsigned short)(uiNumVerts + 1);
        *pusDest++ = (unsigned short)uiNumVerts;

        *pusDest++ = v3;
        *pusDest++ = (unsigned short)(uiNumVerts + 2);
        *pusDest++ = (unsigned short)(uiNumVerts + 1);

        *pusDest++ = (unsigned short)uiNumVerts;
        *pusDest++ = (unsigned short)(uiNumVerts + 1);
        *pusDest++ = (unsigned short)(uiNumVerts + 2);

        // update the number of vertices
        uiNumVerts += 3;
    }

    // swap the two temporary connectivity arrays
    unsigned short* pusTmp = pusConn;
    pusConn = pusConnTmp;
    pusConnTmp = pusTmp;

    // update the number of tris
    uiNumTris *= 4;

    // scale the vertices and translate
    NiPoint3* pkTmpPoints = pkPoints;
    for (i = 0; i < uiNumVerts; i++)
    {
        *pkTmpPoints = (*pkTmpPoints) * kSphere.m_fRadius * fMultExt + 
            kCenter;
        pkTmpPoints++;
    }
    
    NiMesh* pkNewMesh = CreateMeshFromVertexIndexData(pkPoints,
        uiNumVerts, pusConn, uiTotalTris_X_3);

    // free the setup arrays
    NiFree(pusConnTmp);
    NiFree(pusConn);
    NiDelete[] pkPoints;

    //NILOG("Building sphere BV wireframe geometry.\n");
    return pkNewMesh;
}
//---------------------------------------------------------------------------
void NiDrawableBV::CreateFromUnion(const NiUnionBV* pkUnion, 
    NiNode* pkParent, const float fMult)
{
    NIASSERT(pkParent);
    NiMesh* pkMesh;

    for (unsigned int i = 0; i < pkUnion->GetSize(); i++)
    {
        const NiBoundingVolume* pkBound = pkUnion->GetBoundingVolume(i);

        switch (pkBound->Type())
        {
        case NiBoundingVolume::BOX_BV:
            pkMesh = CreateFromBox(((NiBoxBV*)pkBound)->GetBox(), fMult);
            pkParent->AttachChild(pkMesh);
            break;
        case NiBoundingVolume::CAPSULE_BV:
            pkMesh = CreateFromCapsule(
                ((NiCapsuleBV*)pkBound)->GetCapsule(), fMult);
            pkParent->AttachChild(pkMesh);
            break;
        case NiBoundingVolume::SPHERE_BV:
            pkMesh = CreateFromSphere(((NiSphereBV*)pkBound)->GetSphere(),
                fMult);
            pkParent->AttachChild(pkMesh);
            break;
        case NiBoundingVolume::UNION_BV:
            {
                NiNode* pkNode = NiNew NiNode;
                CreateFromUnion((NiUnionBV*)pkBound, pkNode, fMult);
                pkParent->AttachChild(pkNode);
                break;
            }
        default:  // HALFSPACE_BV
            break;
        }
   }
}
//---------------------------------------------------------------------------
NiMesh* NiDrawableBV::CreateTubeFromCapsule(const NiCapsule& kCapsule,
    const float fMult, const unsigned int uiNumSides)
{
    const float fRadius = kCapsule.m_fRadius * fMult;
    NiPoint3 kDirection = kCapsule.m_kSegment.m_kDirection;
    NiPoint3 kEndPt0 = kCapsule.m_kSegment.m_kOrigin;
    NiPoint3 kEndPt1 = kEndPt0 + kDirection;

    // compute unit-length kCapsule kAxis
    NiPoint3 kAxis = kDirection;
    kAxis.Unitize();

    // create two basis vectors that are perpendicular to the kCapsule kAxis
    NiPoint3 kBasis1;
    if (NiAbs(kAxis.x) >= NiAbs(kAxis.y) && NiAbs(kAxis.x) >= NiAbs(kAxis.z))
    {
        kBasis1.x = -kAxis.y;
        kBasis1.y = kAxis.x;
        kBasis1.z = kAxis.z;
    }
    else
    {
        kBasis1.x = kAxis.x;
        kBasis1.y = kAxis.z;
        kBasis1.z = -kAxis.y;
    }

    kBasis1.Unitize();
    NiPoint3 kBasis0 = kBasis1.Cross(kAxis);
    kBasis0 *= fRadius;
    kBasis1 *= fRadius;

    // cross section of kCylinder is uiNumSides-sided regular polygon
    unsigned int uiNumVerts = uiNumSides + uiNumSides;
    unsigned int uiNumTris = uiNumVerts;
    NiPoint3* pkPoints = NiNew NiPoint3[uiNumVerts];
    unsigned short* pusConn = NiAlloc(unsigned short, uiNumTris * 3);

    float fIncr = 2.0f * NI_PI / (float)uiNumSides;
    float fTheta = 0.0f;
    unsigned int i;
    for (i = 0; i < uiNumSides; i++)
    {
        NiPoint3 kOffset = kBasis0 * NiCos(fTheta) + kBasis1 * NiSin(fTheta);

        pkPoints[i] = kEndPt0 + kOffset;
        pkPoints[i + uiNumSides] = kEndPt1 + kOffset;

        fTheta += fIncr;
    }

    unsigned short* pusConnTmp = pusConn;
    for (i = 0; i < uiNumSides - 1; i++)
    {
        *pusConnTmp++ = (unsigned short)i;
        *pusConnTmp++ = (unsigned short)(i + uiNumSides);
        *pusConnTmp++ = (unsigned short)(i + 1 + uiNumSides);

        *pusConnTmp++ = (unsigned short)i;
        *pusConnTmp++ = (unsigned short)(i + 1 + uiNumSides);
        *pusConnTmp++ = (unsigned short)(i + 1);
    }

    *pusConnTmp++ = 0;
    *pusConnTmp++ = (unsigned short)(uiNumSides + uiNumSides - 1);
    *pusConnTmp++ = (unsigned short)(uiNumSides);

    *pusConnTmp++ = (unsigned short)(uiNumSides - 1);
    *pusConnTmp++ = 0;
    *pusConnTmp++ = (unsigned short)(uiNumSides + uiNumSides - 1);

    NiMesh* pkNewMesh = CreateMeshFromVertexIndexData(pkPoints,
        uiNumVerts, pusConn, uiNumTris * 3);

    NiDelete[] pkPoints;
    NiFree(pusConn);

    //NILOG("Converting tube BV to capsule.\n");
    return pkNewMesh;
}
//---------------------------------------------------------------------------
