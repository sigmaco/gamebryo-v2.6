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

#include "NiUnionBV.h"
#include <NiPoint3.h>

//---------------------------------------------------------------------------
NiUnionBV::NiUnionBV() : m_kBoundingVolume(2, 1)
{
    m_uiWhichObjectIntersect = 0;
}
//---------------------------------------------------------------------------
inline NiUnionBV::NiUnionBV(const NiUnionBV& kABV) :
    NiBoundingVolume()
{
    NiBoundingVolume* pkChildABV;

    m_kBoundingVolume.SetSize(kABV.m_kBoundingVolume.GetSize());

    for (unsigned int i = 0; i < kABV.m_kBoundingVolume.GetSize(); i++)
    {
        pkChildABV = kABV.m_kBoundingVolume.GetAt(i);
        if (pkChildABV)
        {
            pkChildABV = pkChildABV->Clone();
            m_kBoundingVolume.SetAt(i, pkChildABV);
        }
    }
}
//---------------------------------------------------------------------------
NiUnionBV::~NiUnionBV()
{
    RemoveAllBoundingVolumes();
}
//---------------------------------------------------------------------------
void NiUnionBV::AddBoundingVolume(NiBoundingVolume* pkABV)
{
    // bounding volume must exist
    if (!pkABV)
        return;

    // prevent adding the same bounding volume more than once
    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        if (pkABV == m_kBoundingVolume.GetAt(i))
            return;
    }

    // Avoid increase in array size of a lot of add/removes occur.  An
    // invariant is that the array remains compact.
    m_kBoundingVolume.AddFirstEmpty(pkABV);
}
//---------------------------------------------------------------------------
void NiUnionBV::RemoveBoundingVolume(NiBoundingVolume* pkABV)
{
    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        if (pkABV == m_kBoundingVolume.GetAt(i))
        {
            NiDelete m_kBoundingVolume.RemoveAt(i);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiUnionBV::RemoveAllBoundingVolumes()
{
    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
        NiDelete m_kBoundingVolume.GetAt(i);

    m_kBoundingVolume.RemoveAll();
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiUnionBV::Create() const 
{ 
    NiUnionBV* pUnion = NiNew NiUnionBV;
    pUnion->m_kBoundingVolume.SetSize(m_kBoundingVolume.GetSize());

    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pBvSrc = m_kBoundingVolume.GetAt(i);
        NiBoundingVolume* pBvDst = pBvSrc->Create();
        pUnion->m_kBoundingVolume.SetAt(i, pBvDst);
    }

    return pUnion; 
}
//---------------------------------------------------------------------------
void NiUnionBV::Copy(const NiBoundingVolume& kABV)
{
    if (kABV.Type() != UNION_BV)
    {
        // copy of non-union BV to union BV is not supported
        NIASSERT(false);
        return;
    }

    NiUnionBV& kUnionABV = (NiUnionBV&) kABV;

    RemoveAllBoundingVolumes();
    m_kBoundingVolume.SetSize(kUnionABV.m_kBoundingVolume.GetSize());

    for (unsigned int i = 0; i < kUnionABV.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pBvSrc = kUnionABV.m_kBoundingVolume.GetAt(i);
        NiBoundingVolume* pBvDst = pBvSrc->Create();
        pBvDst->Copy(*pBvSrc);
        m_kBoundingVolume.SetAt(i, pBvDst);
    }
}
//---------------------------------------------------------------------------
bool NiUnionBV::operator==(const NiBoundingVolume& kABV) const
{
    if (kABV.Type() != UNION_BV)
        return false;

    NiUnionBV& kUnionABV = (NiUnionBV&) kABV;

    if (m_kBoundingVolume.GetSize() != kUnionABV.m_kBoundingVolume.GetSize())
        return false;

    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        if (*m_kBoundingVolume.GetAt(i) != 
            *kUnionABV.m_kBoundingVolume.GetAt(i))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiUnionBV::operator!=(const NiBoundingVolume& kABV) const
{
    return !operator==(kABV);
}
//---------------------------------------------------------------------------
void NiUnionBV::UpdateWorldData(const NiBoundingVolume& kModelABV, 
    const NiTransform& kWorld)
{
    // The only caller of NiUnionBV is the internal collision system.  These
    // conditions are satisfied in every place UpdateWorldData is called.
    NIASSERT(kModelABV.Type() == UNION_BV);
    NiUnionBV& kUnionABV = *(NiUnionBV*)(&kModelABV);
    NIASSERT(m_kBoundingVolume.GetSize() 
        == kUnionABV.m_kBoundingVolume.GetSize());

    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        m_kBoundingVolume.GetAt(i)->UpdateWorldData(
            *kUnionABV.m_kBoundingVolume.GetAt(i), kWorld);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'test' intersections
//---------------------------------------------------------------------------
bool NiUnionBV::UnionOtherTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    NIASSERT(kABV0.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;

    // returns on first found intersection
    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV = kUnionABV0.m_kBoundingVolume.GetAt(i);
        if (NiBoundingVolume::TestIntersect(fTime, *pkABV, kV0, kABV1, kV1))
        {
            kUnionABV0.m_uiWhichObjectIntersect = i; 
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiUnionBV::UnionUnionTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    NIASSERT(kABV0.Type() == UNION_BV && kABV1.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;
    NiUnionBV& kUnionABV1 = (NiUnionBV&) kABV1;

    // returns on first found intersection
    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); 
        i++)
    {
        NiBoundingVolume* pkABV0 = kUnionABV0.m_kBoundingVolume.GetAt(i);
        for (unsigned int j = 0; j < kUnionABV1.m_kBoundingVolume.GetSize(); 
            j++)
        {
            NiBoundingVolume* pkABV1 = kUnionABV1.m_kBoundingVolume.GetAt(j);
            if (NiBoundingVolume::TestIntersect(fTime, *pkABV0, kV0, *pkABV1,
                kV1))
            {
                kUnionABV0.m_uiWhichObjectIntersect = i;
                kUnionABV1.m_uiWhichObjectIntersect = j;
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiUnionBV::UnionTriTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1,  const NiPoint3& kVert2, 
    const NiPoint3& kV1)
{
    NIASSERT(kABV0.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;

    // returns on first found intersection
    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV = kUnionABV0.m_kBoundingVolume.GetAt(i);
        if (NiBoundingVolume::TestTriIntersect(fTime, *pkABV, kV0, kVert0, 
            kVert1, kVert2, kV1))
        {
            kUnionABV0.m_uiWhichObjectIntersect = i;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'find' intersections
//---------------------------------------------------------------------------
bool NiUnionBV::UnionOtherFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    NIASSERT(kABV0.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;

    // returns on earliest found intersection
    bool bIntersect = false;
    float fEarliestTime = FLT_MAX;
    
    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV = kUnionABV0.m_kBoundingVolume.GetAt(i);
        if (NiBoundingVolume::FindIntersect(fTime, *pkABV, kV0, kABV1, kV1, 
            fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1))
        {
            if (fIntrTime < fEarliestTime)
            {
                kUnionABV0.m_uiWhichObjectIntersect = i;
                fEarliestTime = fIntrTime;
                bIntersect = true;
            }
        }
    }

    return bIntersect;
}
//---------------------------------------------------------------------------
bool NiUnionBV::UnionUnionFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    NIASSERT(kABV0.Type() == UNION_BV && kABV1.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;
    NiUnionBV& kUnionABV1 = (NiUnionBV&) kABV1;

    // returns on earliest found intersection
    bool bIntersect = false;
    float fEarliestTime = FLT_MAX;

    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV0 = kUnionABV0.m_kBoundingVolume.GetAt(i);
        for (unsigned int j = 0; j < kUnionABV1.m_kBoundingVolume.GetSize(); 
            j++)
        {
            NiBoundingVolume* pkABV1 = kUnionABV1.m_kBoundingVolume.GetAt(j);
            if (NiBoundingVolume::FindIntersect(fTime, *pkABV0, kV0, *pkABV1,
                kV1, fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1))
            {
                if (fIntrTime < fEarliestTime)
                {
                        kUnionABV0.m_uiWhichObjectIntersect = i;
                        kUnionABV1.m_uiWhichObjectIntersect = j;
                        fEarliestTime = fIntrTime;
                        bIntersect = true;
                }
            }
        }
    }

    return bIntersect;
}
//---------------------------------------------------------------------------
bool NiUnionBV::UnionTriFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2, 
    const NiPoint3& kV1, float& fIntrTime, NiPoint3& kIntrPt, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NIASSERT(kABV0.Type() == UNION_BV);
    NiUnionBV& kUnionABV0 = (NiUnionBV&) kABV0;

    // returns on earliest found intersection
    bool bIntersect = false;
    float fEarliestTime = FLT_MAX;

    for (unsigned int i = 0; i < kUnionABV0.m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV = kUnionABV0.m_kBoundingVolume.GetAt(i);
        if (NiBoundingVolume::FindTriIntersect(fTime, *pkABV, kV0, kVert0, 
            kVert1, kVert2, kV1, fIntrTime, kIntrPt, bCalcNormals, kNormal0, 
            kNormal1))
        {
                if (fIntrTime < fEarliestTime)
                {
                    kUnionABV0.m_uiWhichObjectIntersect = i; 
                    fEarliestTime = fIntrTime;
                    bIntersect = true;
                }
        }
    }

    return bIntersect;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiBoundingVolume* NiUnionBV::Clone() const
{
    NiUnionBV* pClone = NiNew NiUnionBV(*this);
    NIASSERT(pClone);
    return (NiBoundingVolume*) pClone;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiBoundingVolume* NiUnionBV::CreateFromStream(NiStream& stream)
{
    NiUnionBV* pkABV = NiNew NiUnionBV;
    pkABV->LoadBinary(stream);
    return pkABV;
}
//---------------------------------------------------------------------------
void NiUnionBV::LoadBinary(NiStream& stream)
{
    NiBoundingVolume::LoadBinary(stream);

    unsigned int uiSize;
    NiStreamLoadBinary(stream, uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiBoundingVolume* pkABV = NiBoundingVolume::CreateFromStream(stream);
        AddBoundingVolume(pkABV);
    }
}
//---------------------------------------------------------------------------
void NiUnionBV::SaveBinary(NiStream& stream)
{
    NiBoundingVolume::SaveBinary(stream);

    NiStreamSaveBinary(stream, m_kBoundingVolume.GetSize());
    for (unsigned int i = 0; i < m_kBoundingVolume.GetSize(); i++)
    {
        NiBoundingVolume* pkABV = m_kBoundingVolume.GetAt(i);
        if (pkABV)
            pkABV->SaveBinary(stream);
    }
}
//---------------------------------------------------------------------------
void NiUnionBV::AddViewerStrings(const char* pcPrefix, 
    NiViewerStringsArray* pkStrings) const
{
    size_t stLen = strlen(pcPrefix) + 12;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = UNION_BV", pcPrefix);
    pkStrings->Add(pString);

    for (unsigned int i = 0; i < GetSize(); i++)
    {
        char* pChildPrefix = NiAlloc(char, 13);
        NiSprintf(pChildPrefix, 13, "   child %i", i);
        const NiBoundingVolume* pkABV = GetBoundingVolume(i);
        pkABV->AddViewerStrings(pChildPrefix, pkStrings);
    }

    return;
}
//---------------------------------------------------------------------------
