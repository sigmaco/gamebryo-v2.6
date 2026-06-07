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
#include "NiAnimationPCH.h"

#include <NiStream.h>
#include <NiMath.h>
#include "NiPosKey.h"

NiImplementAnimationStream(NiPosKey,POSKEY,NOINTERP);

// default sample size for computing maximum curvature
int NiPosKey::ms_iSampleSize = 20;
float NiPosKey::ms_fSampleDelta = 1.0f/ms_iSampleSize;
NiPoint3 NiPosKey::ms_kDefault = NiPoint3::ZERO;

//---------------------------------------------------------------------------
void NiPosKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, ComputeMaxCurvature);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, InterpolateD1);
    SetInterpD2Function(eContent, eType, InterpolateD2);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, NULL);
    SetCubicCoefsFunction(eContent, eType, NULL);
    SetIsPosedFunction(eContent, eType, NULL);
}
//---------------------------------------------------------------------------
NiPoint3 NiPosKey::GenInterp(float fTime, NiPosKey* pkKeys,
    NiPosKey::KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
        return pkKeys->GetKeyAt(0, ucSize)->GetPos();

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    float fLastTime = pkKeys->GetKeyAt(uiLastIdx, ucSize)->GetTime();
    if ( fTime < fLastTime )
    {
        uiLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }
    
    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }

    NIASSERT(uiNextIdx < uiNumKeys);

    // interpolate the keys, requires that the time is normalized to [0,1]
    float fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);
    NiPosKey::InterpFunction interp = NiPosKey::GetInterpFunction(eType);
    NIASSERT( interp );
    NiPoint3 kResult;
    interp(fNormTime, pkKeys->GetKeyAt(uiLastIdx, ucSize),
        pkKeys->GetKeyAt(uiNextIdx, ucSize), &kResult);
    return kResult;
}
//---------------------------------------------------------------------------
void NiPosKey::GenCubicCoefs(float fTime, NiPosKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize, float& fTime0, float& fTime1, 
    NiPoint3& kValue0, NiPoint3& kOutTangent0,
    NiPoint3& kA0, NiPoint3& kB0)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
    {
        // Hold this constant value forever.
        kValue0 = pkKeys->GetKeyAt(0, ucSize)->GetPos();
        kOutTangent0 = NiPoint3::ZERO;
        kA0 = NiPoint3::ZERO;
        kB0 = NiPoint3::ZERO;
        fTime0 = 0.0f;
        fTime1 = NI_INFINITY;
        uiLastIdx = 0;
        return;
    }

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    // Copy the last index to a stack variable here to ensure that each thread
    // has its own consistent copy of the value. The stack variable is copied
    // back to the reference variable at the end of this function.
    unsigned int uiStackLastIdx = uiLastIdx;

    float fLastTime = pkKeys->GetKeyAt(uiStackLastIdx, ucSize)->GetTime();
    if (fTime < fLastTime)
    {
        uiStackLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }

    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for(uiNextIdx = uiStackLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();

        // Don't allow step functions to halt when the 
        // time matches the second key.
        if ((fTime < fNextTime) || (fTime == fNextTime && eType != STEPKEY))
        {
            break;
        }

        uiStackLastIdx++;
        fLastTime = fNextTime;
    }
    
    if (eType == STEPKEY && fTime >= fNextTime && uiNextIdx == uiNumKeys)
    {
        // Step function is at the last key.
        kValue0 = pkKeys->GetKeyAt(uiNumKeysM1, ucSize)->GetPos();
        kOutTangent0 = NiPoint3::ZERO;
        kA0 = NiPoint3::ZERO;
        kB0 = NiPoint3::ZERO;
        fTime0 = fNextTime;
        fTime1 = fNextTime;
        uiLastIdx = uiNumKeysM1;
    }
    else
    {
        // Extract the cubic coefficients from the keys.
        NIASSERT(uiNextIdx < uiNumKeys);
        NiAnimationKey::CubicCoefsFunction cubiccoefs = 
            NiPosKey::GetCubicCoefsFunction(eType);
        NIASSERT(cubiccoefs);
        cubiccoefs(pkKeys, uiNumKeys, uiStackLastIdx, &kValue0, 
            &kOutTangent0, &kA0, &kB0);
        fTime0 = fLastTime;
        fTime1 = fNextTime;
        uiLastIdx = uiStackLastIdx;
    }
}
//---------------------------------------------------------------------------
NiPoint3 NiPosKey::GenInterpDefault(float fTime, NiPosKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned char ucSize)
{
    if (uiNumKeys)
    {
        // GenInterp no longer handles out-of-range times
        if (fTime < pkKeys->GetKeyAt(0, ucSize)->GetTime())
            return pkKeys->GetKeyAt(0, ucSize)->GetPos();
        else if (fTime > pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime())
            return pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetPos();
        else
        {
            unsigned int uiLastIdx = 0;
            return NiPosKey::GenInterp(fTime, pkKeys, eType, uiNumKeys,
                uiLastIdx, ucSize);
        }
    }
    else
    {
        return ms_kDefault;
    }
}
//---------------------------------------------------------------------------
void NiPosKey::SetDefault(const NiPoint3& kDefault)
{
    ms_kDefault = kDefault;
}
//---------------------------------------------------------------------------
bool NiPosKey::Equal(const NiAnimationKey& key0, const NiAnimationKey& key1)
{
    if ( !NiAnimationKey::Equal(key0,key1) )
        return false;

    const NiPosKey& pos0 = (const NiPosKey&) key0;
    const NiPosKey& pos1 = (const NiPosKey&) key1;

    return pos0.m_Pos == pos1.m_Pos;
}
//---------------------------------------------------------------------------
void NiPosKey::CoordinateFrame(float fTime, const NiPosKey* pKey0,
    const NiPosKey* pKey1, NiPosKey::KeyType eType, NiPoint3& tangent,
    NiPoint3& normal, NiPoint3& binormal, float& curvature)
{
    InterpFunction d1Func = NiPosKey::GetInterpD1Function(eType);
    InterpFunction d2Func = NiPosKey::GetInterpD2Function(eType);
    
    NiPoint3 d1, d2;
    d1Func(fTime,pKey0,pKey1,&d1);  // assert: d1 is not zero
    d2Func(fTime,pKey0,pKey1,&d2);

    // compute unit-length tangent vector
    float fLengthSqr = d1.SqrLength();
    float fLength = NiSqrt(fLengthSqr);
    float fInvLength = 1.0f/fLength;
    tangent = fInvLength*d1;

    // NOTE.  Let L = Length(d1).  Curvature is K = Length(Cross(d1,d2))/L^3.
    // The pseudo-curvature K*L is a dimensionless quantity (invariant to
    // changes in units of measurements) and is used to decide how to compute
    // a normal vector to the curve.  K*L is returned instead of K.
    NiPoint3 cross = d1.Cross(d2);
    curvature = cross.Length()*fInvLength*fInvLength;

    // compute unit-length normal vector
    const float epsilon = 1e-06f;
    if ( NiAbs(curvature) > epsilon )
    {
        normal = fLengthSqr*d2 - d1.Dot(d2)*d1;
    }
    else
    {
        curvature = 0.0f;
        if ( NiAbs(tangent.x) > epsilon || NiAbs(tangent.y) > epsilon )
        {
            normal.x = tangent.y;
            normal.y = -tangent.x;
            normal.z = 0;
        }
        else
        {
            normal.z = tangent.y;
            normal.y = -tangent.z;
            normal.x = 0;
        }
    }
    normal.Unitize();

    // compute unit-length binormal (TxN is already unit length)
    binormal = tangent.Cross(normal);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiPosKey::CreateFromStream(NiStream&, unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiPosKey::LoadBinary(NiStream& stream)
{
    NiAnimationKey::LoadBinary(stream);

    m_Pos.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiPosKey::SaveToStream(NiStream&, NiAnimationKey*,
    unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiPosKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiAnimationKey::SaveBinary(stream, pkKey);

    NiPosKey* pkPos = (NiPosKey*) pkKey;
    pkPos->m_Pos.SaveBinary(stream);
}
//---------------------------------------------------------------------------

