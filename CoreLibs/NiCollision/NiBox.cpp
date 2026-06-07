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

#include <NiMatrix3.h>
#include <NiMesh.h>
#include "NiBox.h"
#include <NiTPointerMap.h>

//---------------------------------------------------------------------------
NiPoint3 NiBox::GetPoint(float fP0, float fP1, float fP2) const
{
    // to be in kBox, need |fPi| <= m_afExtent[i]
    return m_kCenter + (fP0 * m_akAxis[0]) + (fP1 * m_akAxis[1]) 
        + (fP2 * m_akAxis[2]);
}
//---------------------------------------------------------------------------
bool NiBox::operator==(const NiBox& kBox) const
{
    if (m_kCenter != kBox.m_kCenter)
        return false;

    for (int i = 0; i < 3; i++)
    {
        if ((m_akAxis[i] != kBox.m_akAxis[i]) || 
            (m_afExtent[i] != kBox.m_afExtent[i]))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBox::operator!=(const NiBox& kBox) const
{
    return !operator==(kBox);
}
//---------------------------------------------------------------------------
void NiBox::InitializeMinMaxAndCenter(NiDataStreamRef*, 
    NiDataStreamElementLock& kLockPosition, NiUInt32, 
    float& fMinX, float& fMaxX, float& fMinY, float& fMaxY, float& fMinZ,
    float& fMaxZ)
{
    // Retrieve first index and initialize min/max
    NIASSERT(kLockPosition.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kFirst =
        kLockPosition.begin<NiPoint3>();
    NiPoint3 kFirstVertex = *kFirst;

    fMinX = fMaxX = kFirstVertex.x;
    fMinY = fMaxY = kFirstVertex.y;
    fMinZ = fMaxZ = kFirstVertex.z;

    m_kCenter = kFirstVertex;
}
//---------------------------------------------------------------------------
void NiBox::CalculateMinMaxAndCenter(NiDataStreamRef*, 
    NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount,
    float& fMinX, float& fMaxX, float& fMinY, float& fMaxY, float& fMinZ,
    float& fMaxZ)
{
    NIASSERT(kLockPosition.IsLocked());

    for (NiUInt32 ui=0; ui < uiSubmeshCount; ui++)
    {
        // These iterators should be just over the "region"
        NiTStridedRandomAccessIterator<NiPoint3> kBegin =
            kLockPosition.begin<NiPoint3>(ui);
        NiTStridedRandomAccessIterator<NiPoint3> kEnd =
            kLockPosition.end<NiPoint3>(ui);

        for (NiTStridedRandomAccessIterator<NiPoint3> kIter = kBegin; 
            kIter!= kEnd; ++kIter)
        {
            NiPoint3 kVertex = *kIter;

            m_kCenter += kVertex;

            if (kVertex.x < fMinX)
                fMinX = kVertex.x;
            else if (kVertex.x > fMaxX)
                fMaxX = kVertex.x;

            if (kVertex.y < fMinY)
                fMinY = kVertex.y;
            else if (kVertex.y > fMaxY)
                fMaxY = kVertex.y;

            if (kVertex.z < fMinZ)
                fMinZ = kVertex.z;
            else if (kVertex.z > fMaxZ)
                fMaxZ = kVertex.z;
        }
        m_kCenter /= (float)kLockPosition.count();
    }
}
//---------------------------------------------------------------------------
void NiBox::ComputeCovarianceAndSolve(NiDataStreamRef*, 
    NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount)
{
    NiMatrix3 kCovar = NiMatrix3::ZERO;
    NIASSERT(kLockPosition.IsLocked());

    for (NiUInt32 ui = 0; ui < uiSubmeshCount; ui++)
    {
        unsigned int uiQuantity = kLockPosition.count(ui);

        kCovar.SetEntry(0, 0, 
            kCovar.GetEntry(0,0) + uiQuantity * (m_kCenter.x * m_kCenter.x));
        kCovar.SetEntry(0, 1, 
            kCovar.GetEntry(0,1) + uiQuantity * (m_kCenter.x * m_kCenter.y));
        kCovar.SetEntry(0, 2, 
            kCovar.GetEntry(0,2) + uiQuantity * (m_kCenter.x * m_kCenter.z));
        kCovar.SetEntry(1, 1, 
            kCovar.GetEntry(1,1) + uiQuantity * (m_kCenter.y * m_kCenter.y));
        kCovar.SetEntry(1, 2, 
            kCovar.GetEntry(1,2) + uiQuantity * (m_kCenter.y * m_kCenter.z));
        kCovar.SetEntry(2, 2, 
            kCovar.GetEntry(2,2) + uiQuantity * (m_kCenter.z * m_kCenter.z));
    }
    kCovar.SetEntry(1, 0, kCovar.GetEntry(0, 1));
    kCovar.SetEntry(2, 0, kCovar.GetEntry(0, 2));
    kCovar.SetEntry(2, 1, kCovar.GetEntry(1, 2));

    float afEigenvalue[3];
    kCovar.EigenSolveSymmetric(afEigenvalue, m_akAxis);
}
//---------------------------------------------------------------------------
void NiBox::ComputeCenterAndExtent(NiDataStreamRef*,
    NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount)
{
    NIASSERT(kLockPosition.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kFirst =
        kLockPosition.begin<NiPoint3>(0);
    NiPoint3 kFirstVertex = *kFirst;

    // Box axes are the eigenvectors of the covariance matrix with adjusted
    // lengths to enclose the points.
    NiPoint3 kMin = kFirstVertex - m_kCenter;
    NiPoint3 kMax = kMin;

    for (NiUInt32 ui = 0; ui < uiSubmeshCount; ui++)
    { 
        // These iterators should be just over the "region"
        NiTStridedRandomAccessIterator<NiPoint3> kBegin =
            kLockPosition.begin<NiPoint3>(ui);
        NiTStridedRandomAccessIterator<NiPoint3> kEnd =
            kLockPosition.end<NiPoint3>(ui);

        for (NiTStridedRandomAccessIterator<NiPoint3> kIter = kBegin; 
            kIter!= kEnd; ++kIter)
        {
            NiPoint3 kVertex = *kIter;
            NiPoint3 kDiff = kVertex - m_kCenter;

            float fDot = kDiff.Dot(m_akAxis[0]);
            if (fDot < kMin.x)
                kMin.x = fDot;
            else if (fDot > kMax.x)
                kMax.x = fDot;

            fDot = kDiff.Dot(m_akAxis[1]);
            if (fDot < kMin.y)
                kMin.y = fDot;
            else if (fDot > kMax.y)
                kMax.y = fDot;

            fDot = kDiff.Dot(m_akAxis[2]);
            if (fDot < kMin.z)
                kMin.z = fDot;
            else if (fDot > kMax.z)
                kMax.z = fDot;
        }
    }

    m_kCenter += (0.5f * (kMax.x + kMin.x)) * m_akAxis[0];
    m_kCenter += (0.5f * (kMax.y + kMin.y)) * m_akAxis[1];
    m_kCenter += (0.5f * (kMax.z + kMin.z)) * m_akAxis[2];
    m_afExtent[0] = 0.5f * (kMax.x - kMin.x);
    m_afExtent[1] = 0.5f * (kMax.y - kMin.y);
    m_afExtent[2] = 0.5f * (kMax.z - kMin.z);
}
//---------------------------------------------------------------------------
bool NiBox::IsDataDegenerate(float fMinX, float fMaxX, float fMinY,
    float fMaxY, float fMinZ, float fMaxZ)
{
    // Check if data is volumetric
    if (fMinX == fMaxX || fMinY == fMaxY || fMinZ == fMaxZ)
    {
        // This case is degenerate, and there is no strategy for optimizing
        // the bounding box, other than using axis alignment.  Also, to give
        // the box some depth, an arbituary value of 0.1 is used.
        m_akAxis[0] = NiPoint3(1.0f, 0.0f, 0.0f);
        m_akAxis[1] = NiPoint3(0.0f, 1.0f, 0.0f);
        m_akAxis[2] = NiPoint3(0.0f, 0.0f, 1.0f);
        float fXLength = (fMaxX - fMinX) * 0.5f;
        float fYLength = (fMaxY - fMinY) * 0.5f;
        float fZLength = (fMaxZ - fMinZ) * 0.5f;

        if (fXLength <= 0.0f)
            fXLength = 0.1f;
        if (fYLength <= 0.0f)
            fYLength = 0.1f;
        if (fZLength <= 0.0f)
            fZLength = 0.1f;

        m_afExtent[0] = fXLength;
        m_afExtent[1] = fYLength;
        m_afExtent[2] = fZLength;

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiBox::CreateFromData(NiMesh* pkMesh)
{
    NIASSERT(pkMesh->GetVertexCount() > 0);

    NiDataStreamRef* pkStreamRef = NULL;
    NiDataStreamElement kElement;

    // The collision detection system requires 32-bit floats for vertex
    // coordinates, but that's the default data type for export from the art
    // packages.
    if (pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        pkStreamRef, kElement) == false)
    {
        NIASSERT(!"Data not appropriate to build NiBox");
        return;
    }

    // Retrieve the number of submeshes.
    NiUInt32 uiSubmeshCount = pkMesh->GetSubmeshCount();

    NiDataStreamElementLock kLockPosition(pkMesh,
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_READ);

    // [1] Initializes min/max and m_kCenter
    float fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ;
    InitializeMinMaxAndCenter(pkStreamRef, kLockPosition, uiSubmeshCount,
        fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ);

    // [2] Calculate min/max values across all submeshes
    CalculateMinMaxAndCenter(pkStreamRef, kLockPosition, uiSubmeshCount,
        fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ);

    // [3] Check if data is Volumetric across all submeshes
    if (IsDataDegenerate(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ))
        return;

    // [4] Compute covariance of points across all submeshes
    ComputeCovarianceAndSolve(pkStreamRef, kLockPosition, uiSubmeshCount);

    // [5] Adjust min/max across all submeshes.
    ComputeCenterAndExtent(pkStreamRef, kLockPosition, uiSubmeshCount);
}
//---------------------------------------------------------------------------
void NiBox::CreateFromData(unsigned short usQuantity, 
    const NiPoint3* pkVertex)
{
    NIASSERT(usQuantity > 0 && pkVertex);

    // Compute mean of points.
    float fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ;
    fMinX = fMaxX = pkVertex[0].x;
    fMinY = fMaxY = pkVertex[0].y;
    fMinZ = fMaxZ = pkVertex[0].z;
    m_kCenter = pkVertex[0];
    unsigned short i;
    for (i = 1; i < usQuantity; i++)
    {
        m_kCenter += pkVertex[i];

        if (pkVertex[i].x < fMinX)
            fMinX = pkVertex[i].x;
        else if (pkVertex[i].x > fMaxX)
            fMaxX = pkVertex[i].x;

        if (pkVertex[i].y < fMinY)
            fMinY = pkVertex[i].y;
        else if (pkVertex[i].y > fMaxY)
            fMaxY = pkVertex[i].y;

        if (pkVertex[i].z < fMinZ)
            fMinZ = pkVertex[i].z;
        else if (pkVertex[i].z > fMaxZ)
            fMaxZ = pkVertex[i].z;
    }

    m_kCenter = m_kCenter / ((float)(usQuantity));

    if (IsDataDegenerate(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ))
        return;

    // Compute covariances of points.  The division by quantity is not
    // necessary since the eigenvalues of the matrix are not used, only the
    // eigenvectors.
    NiMatrix3 kCovar = NiMatrix3::ZERO;

    kCovar.SetEntry(0, 0, kCovar.GetEntry(0,0) + (m_kCenter.x * m_kCenter.x) *
        usQuantity);
    kCovar.SetEntry(0, 1, kCovar.GetEntry(0,1) + (m_kCenter.x * m_kCenter.y) *
        usQuantity);
    kCovar.SetEntry(0, 2, kCovar.GetEntry(0,2) + (m_kCenter.x * m_kCenter.z) *
        usQuantity);
    kCovar.SetEntry(1, 1, kCovar.GetEntry(1,1) + (m_kCenter.y * m_kCenter.y) *
        usQuantity);
    kCovar.SetEntry(1, 2, kCovar.GetEntry(1,2) + (m_kCenter.y * m_kCenter.z) *
        usQuantity);
    kCovar.SetEntry(2, 2, kCovar.GetEntry(2,2) + (m_kCenter.z * m_kCenter.z) *
        usQuantity);

    kCovar.SetEntry(1, 0, kCovar.GetEntry(0, 1));
    kCovar.SetEntry(2, 0, kCovar.GetEntry(0, 2));
    kCovar.SetEntry(2, 1, kCovar.GetEntry(1, 2));

    float afEigenvalue[3];
    kCovar.EigenSolveSymmetric(afEigenvalue, m_akAxis);

    // Box axes are the eigenvectors of the covariance matrix with adjusted
    // lengths to enclose the points.
    NiPoint3 kMin = pkVertex[0] - m_kCenter;
    NiPoint3 kMax = kMin;
    for (i = 1; i < usQuantity; i++)
    {
        NiPoint3 kDiff = pkVertex[i] - m_kCenter;

        float fDot = kDiff.Dot(m_akAxis[0]);
        if (fDot < kMin.x)
            kMin.x = fDot;
        else if (fDot > kMax.x)
            kMax.x = fDot;

        fDot = kDiff.Dot(m_akAxis[1]);
        if (fDot < kMin.y)
            kMin.y = fDot;
        else if (fDot > kMax.y)
            kMax.y = fDot;

        fDot = kDiff.Dot(m_akAxis[2]);
        if (fDot < kMin.z)
            kMin.z = fDot;
        else if (fDot > kMax.z)
            kMax.z = fDot;
    }

    m_kCenter += (0.5f * (kMax.x + kMin.x)) * m_akAxis[0];
    m_kCenter += (0.5f * (kMax.y + kMin.y)) * m_akAxis[1];
    m_kCenter += (0.5f * (kMax.z + kMin.z)) * m_akAxis[2];
    m_afExtent[0] = 0.5f * (kMax.x - kMin.x);
    m_afExtent[1] = 0.5f * (kMax.y - kMin.y);
    m_afExtent[2] = 0.5f * (kMax.z - kMin.z);
}
//---------------------------------------------------------------------------
void NiBox::LoadBinary(NiStream& stream)
{
    m_kCenter.LoadBinary(stream);
    for (int i = 0; i < 3; i++)
        m_akAxis[i].LoadBinary(stream);

    NiStreamLoadBinary(stream, m_afExtent, 3);
}
//---------------------------------------------------------------------------
void NiBox::SaveBinary(NiStream& stream)
{
    m_kCenter.SaveBinary(stream);
    for (int i = 0; i < 3; i++)
        m_akAxis[i].SaveBinary(stream);

    NiStreamSaveBinary(stream, m_afExtent, 3);
}
//---------------------------------------------------------------------------
