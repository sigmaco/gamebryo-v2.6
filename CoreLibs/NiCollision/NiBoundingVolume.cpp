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

#include <NiBound.h>
#include "NiBoundingVolume.h"
#include <NiRTLib.h>
#include <NiMesh.h>


NiBVTestIntersectFunction* NiBoundingVolume::ms_pfnTestFuncTable = 0;
NiBVFindIntersectFunction* NiBoundingVolume::ms_pfnFindFuncTable = 0;
NiBVTriTestIntersectFunction* NiBoundingVolume::ms_pfnTriTestFuncTable = 0;
NiBVTriFindIntersectFunction* NiBoundingVolume::ms_pfnTriFindFuncTable = 0;
NiBoundingVolume* (*NiBoundingVolume::ms_pfnConvertToSphereBV_Fast)(float, 
    const NiPoint3&) = 0;
NiBoundingVolume* (*NiBoundingVolume::ms_pfnConvertToSphereBV_Safe)(float, 
    const NiPoint3&) = 0;
NiBoundingVolume* (*NiBoundingVolume::ms_pfnConvertToBoxBV)(unsigned short, 
    const NiPoint3* pkVertex) = 0;
NiBoundingVolume* (*NiBoundingVolume::ms_pfnConvertToBoxBV_FromMesh)(
    NiMesh* pkMesh) = 0;


// kStream management
NiBoundingVolume::CreateFunction
    NiBoundingVolume::ms_apfnLoaders[NiBoundingVolume::MAXTYPE_BV];

//---------------------------------------------------------------------------
bool NiBoundingVolume::TestIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kVel1)
{
    NiBVTestIntersectFunction pFunc; 

    pFunc = ms_pfnTestFuncTable[kBV0.Type() * MAXTYPE_BV + kBV1.Type()];
    if (pFunc)
        return pFunc(fTime, kBV0, kVel0, kBV1, kVel1);
    
    // Only the top "half" of the function table will have non-zero entries.
    // If pFunc is not found in the first search, then its recipricol better 
    // be valid, so use that entry.
    
    pFunc = ms_pfnTestFuncTable[kBV1.Type() * MAXTYPE_BV + kBV0.Type()];
    NIASSERT(pFunc);
    return pFunc(fTime, kBV1, kVel1, kBV0, kVel0);
}
//---------------------------------------------------------------------------
bool NiBoundingVolume::FindIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kVel1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    NiBVFindIntersectFunction pFunc;
    pFunc = ms_pfnFindFuncTable[kBV0.Type() * MAXTYPE_BV + kBV1.Type()];
    if (pFunc)
    {
        return pFunc(fTime, kBV0, kVel0, kBV1, kVel1, fIntrTime, kIntrPt, 
             bCalcNormals, kNormal0, kNormal1);
    }

    // Only the top "half" of the function table will have non-zero entries.
    // If pFunc is not found in the first search, then its recipricol better 
    // be valid, so use that entry.

    pFunc = ms_pfnFindFuncTable[kBV1.Type() * MAXTYPE_BV + kBV0.Type()];
    NIASSERT(pFunc);
    return pFunc(fTime, kBV1, kVel1, kBV0, kVel0, fIntrTime, kIntrPt, 
         bCalcNormals, kNormal1, kNormal0);
}
//---------------------------------------------------------------------------
bool NiBoundingVolume::TestTriIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2, 
    const NiPoint3& kVel1)
{
    NiBVTriTestIntersectFunction pFunc = ms_pfnTriTestFuncTable[kBV0.Type()];
    NIASSERT(pFunc);
    return pFunc(fTime, kBV0, kVel0, kVert0, kVert1, kVert2, kVel1);
}
//---------------------------------------------------------------------------
bool NiBoundingVolume::FindTriIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2, 
    const NiPoint3& kVel1, float& fIntrTime, NiPoint3& kIntrPt, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NiBVTriFindIntersectFunction pFunc = ms_pfnTriFindFuncTable[kBV0.Type()];
    NIASSERT(pFunc);
    return pFunc(fTime, kBV0, kVel0, kVert0, kVert1, kVert2, kVel1, 
         fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1);
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoundingVolume::ConvertToSphereBV_Fast(
    const NiBound& kBound)
{
    if (ms_pfnConvertToSphereBV_Fast) // Collision library has been linked in.
    {
        return ms_pfnConvertToSphereBV_Fast(kBound.GetRadius(),
            kBound.GetCenter());
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoundingVolume::ConvertToSphereBV_Safe(
    const NiBound& kBound)
{
    if (ms_pfnConvertToSphereBV_Safe) // Collision library has been linked in.
    {
        return ms_pfnConvertToSphereBV_Safe(kBound.GetRadius(), 
            kBound.GetCenter());
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoundingVolume::ConvertToBoxBV(unsigned short usQuantity, 
    const NiPoint3* pkVertex)
{
    if (ms_pfnConvertToBoxBV)   // Collision library has been linked in.
        return ms_pfnConvertToBoxBV(usQuantity, pkVertex);
    else
        return 0;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoundingVolume::ConvertToBoxBV(NiMesh* pkMesh)
{
    // collision library has been linked in.
    if (ms_pfnConvertToBoxBV_FromMesh)
        return ms_pfnConvertToBoxBV_FromMesh(pkMesh);
    else
        return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoundingVolume::CreateFromStream(NiStream& kStream)
{
    int iType;
    NiStreamLoadEnum(kStream, iType);
    NIASSERT(iType != -1 && iType < MAXTYPE_BV);
    if (iType < 0 || iType >= MAXTYPE_BV)
        return 0;

    NiBoundingVolume::CreateFunction fnCF = ms_apfnLoaders[iType];
    NIASSERT(fnCF);
    return fnCF(kStream);
}
//---------------------------------------------------------------------------
void NiBoundingVolume::LoadBinary(NiStream&)
{
    // 'type' is loaded by CreateFromStream
}
//---------------------------------------------------------------------------
void NiBoundingVolume::SaveBinary(NiStream& kStream)
{
    // save run-time type information
    NiStreamSaveBinary(kStream, Type());
}
//---------------------------------------------------------------------------

