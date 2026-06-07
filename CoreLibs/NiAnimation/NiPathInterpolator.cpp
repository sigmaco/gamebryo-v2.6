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

#include "NiPathInterpolator.h"
#include "NiQuatTransform.h"

NiImplementRTTI(NiPathInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiPathInterpolator::NiPathInterpolator() :
    m_spPathData(NULL), 
    m_spPctData(NULL), 
    m_kRotation(INVALID_QUATERNION),
    m_kPosition(INVALID_POINT3)
{
    m_uFlags = 0;
    m_uiLastPathIdx = 0;
    m_uiLastPctIdx = 0;

    SetAllowFlip(false);
    SetConstVelocity(false);
    SetFollow(false);
    m_fMaxBankAngle = 0.0f;
    m_fSmoothing = 0.0f;
    m_eBankDir = POSITIVE;
    m_sFollowAxis = 0;
    SetFlip(false);
    SetCurveTypeOpen(true);
    m_fMaxCurvature = 0.0f;

    // constant velocity data
    m_pfPartialLength = 0;
    m_fTotalLength = -1.0f;
    SetCVDataNeedsUpdate(true);
    m_fLastTime = -NI_INFINITY;
}
//---------------------------------------------------------------------------
NiPathInterpolator::NiPathInterpolator(NiPosData* pkPathData,
    NiFloatData* pkPctData) :
    m_spPathData(pkPathData), 
    m_spPctData(pkPctData), 
    m_kRotation(INVALID_QUATERNION),
    m_kPosition(INVALID_POINT3)
{
    m_uFlags = 0;
    m_uiLastPathIdx = 0;
    m_uiLastPctIdx = 0;

    SetAllowFlip(false);
    SetConstVelocity(false);
    SetFollow(false);
    m_fMaxBankAngle = 0.0f;
    m_fSmoothing = 0.0f;
    m_eBankDir = POSITIVE;
    m_sFollowAxis = 0;
    SetFlip(false);
    SetCurveTypeOpen(true);
    m_fMaxCurvature = 0.0f;

    // constant velocity data
    m_pfPartialLength = 0;
    m_fTotalLength = -1.0f;
    SetCVDataNeedsUpdate(true);
    m_fLastTime = -NI_INFINITY;
}
//---------------------------------------------------------------------------
NiPathInterpolator::~NiPathInterpolator()
{
    m_spPathData = 0;
    m_spPctData = 0;
    NiFree(m_pfPartialLength);
}
//---------------------------------------------------------------------------
unsigned short NiPathInterpolator::GetKeyChannelCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
unsigned int NiPathInterpolator::GetKeyCount(unsigned short usChannel) const
{
    if (usChannel == PATH)
    {
        if (!m_spPathData)
            return 0;
        else
            return m_spPathData->GetNumKeys();
    }
    else if (usChannel == PERCENTAGE)
    {
        if (!m_spPctData)
            return 0;
        else
            return m_spPctData->GetNumKeys();
    }
    else
    {
        NIASSERT(usChannel < 2);
        return 0;
    }
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiPathInterpolator::GetKeyType(
    unsigned short usChannel) const
{
    if (usChannel == PATH)
    {
        if (!m_spPathData)
            return NiAnimationKey::NOINTERP;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spPathData->GetAnim(uiNumKeys, eType, ucSize);
        return eType; 
    }
    else if (usChannel == PERCENTAGE)
    {
        if (!m_spPctData)
            return NiAnimationKey::NOINTERP;

        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spPctData->GetAnim(uiNumKeys, eType, ucSize);
        return eType;     
    }
    else
    {
        NIASSERT(usChannel < 2);
        return NiAnimationKey::NOINTERP;
    }
    
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiPathInterpolator::GetKeyContent(
    unsigned short usChannel) const
{
    if (usChannel == PATH)
        return NiAnimationKey::POSKEY;
    else if (usChannel == PERCENTAGE)
        return NiAnimationKey::FLOATKEY;
    
    NIASSERT(usChannel < 2);
    return NiAnimationKey::NUMKEYCONTENTS;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiPathInterpolator::GetKeyArray(unsigned short usChannel)
    const
{
    if (usChannel == PATH)
    {       
        if (!m_spPathData)
            return NULL;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey * pkKeys = m_spPathData->GetAnim(uiNumKeys, eType, 
            ucSize);
        return pkKeys; 
    }
    else if (usChannel == PERCENTAGE)
    {
        if (!m_spPctData)
            return NULL;
    
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        NiFloatKey * pkKeys = m_spPctData->GetAnim(uiNumKeys, eType, 
            ucSize);
        return pkKeys; 
    }
    else
    {
        NIASSERT(usChannel < 2);
        return NULL;
    }
}
//---------------------------------------------------------------------------
unsigned char NiPathInterpolator::GetKeyStride(unsigned short usChannel)
    const
{
    if (usChannel == PATH)
    {       
        if (!m_spPathData)
            return 0;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spPathData->GetAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else if (usChannel == PERCENTAGE)
    {
        if (!m_spPctData)
            return 0;
    
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spPctData->GetAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else
    {
        NIASSERT(usChannel < 2);
        return 0;
    }
}
//---------------------------------------------------------------------------
void NiPathInterpolator::ReplacePathKeys(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (!m_spPathData)
        m_spPathData = NiNew NiPosData;

    m_spPathData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    unsigned char ucSize = NiPosKey::GetKeySize(eType);
    m_uiLastPathIdx = 0;

    // determine if curve is open or closed
    SetCurveTypeOpen(pkKeys->GetKeyAt(0, ucSize)->GetPos() !=
        pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetPos());

    // preserve constant velocity status
    SetCVDataNeedsUpdate(true);
    if (GetConstVelocity())
        SetConstantVelocityData();

    // approximate the maximum curvature of path
    NiPosKey::CurvatureFunction pfnCF;
    pfnCF = NiPosKey::GetCurvatureFunction(eType);
    m_fMaxCurvature = pfnCF((NiAnimationKey*) pkKeys, uiNumKeys);
}
//---------------------------------------------------------------------------
void NiPathInterpolator::ReplacePctKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (!m_spPctData)
        m_spPctData = NiNew NiFloatData;

    m_spPctData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    m_uiLastPctIdx = 0;
}
//---------------------------------------------------------------------------
float NiPathInterpolator::GetTotalPathLength() const
{
    if (m_fTotalLength < 0.0f)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        unsigned char ucSize;
        GetPathKeys(uiNumPathKeys, ePathType, ucSize);
        NIASSERT(uiNumPathKeys >= 1);
        NiFree(m_pfPartialLength);
        m_pfPartialLength = NiAlloc(float,uiNumPathKeys);
        NIASSERT(m_pfPartialLength);

        m_pfPartialLength[0] = 0.0f;
        for (unsigned int ui0 = 0, ui1 = 1; ui0 < uiNumPathKeys-1; ui0++,
            ui1++)
        {
            // arc length of curve between knots i0 and i1
            float fLength = PathLength(ui0,ui1,1.0f);
    
            // length of curve between knots 0 and i1
            m_pfPartialLength[ui1] = m_pfPartialLength[ui0] + fLength;
        }
        m_fTotalLength = m_pfPartialLength[uiNumPathKeys-1];
    }
    return m_fTotalLength;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::SetConstantVelocityData()
{
    if (!GetCVDataNeedsUpdate())
        return;

    m_fTotalLength = GetTotalPathLength();

    SetCVDataNeedsUpdate(false);
}
//---------------------------------------------------------------------------
bool NiPathInterpolator::Update(float fTime, NiObjectNET*, 
    NiQuatTransform& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        if (m_kPosition == INVALID_POINT3)
        {
            kValue.MakeInvalid();
            return false;
        }

        kValue.SetTranslate(m_kPosition);
        if (GetFollow())
            kValue.SetRotate(m_kRotation);
        return true;
    }

    unsigned int uiNumPctKeys;
    NiFloatKey::KeyType ePctType;
    unsigned char ucPctSize;
    NiFloatKey* pkPctKeys = GetPctKeys(uiNumPctKeys, ePctType, ucPctSize);
    if (!pkPctKeys)
        return false;

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucPathSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucPathSize);
    if (!pkPathKeys)
        return false;

    float fPct = NiFloatKey::GenInterp(fTime, pkPctKeys, ePctType,
        uiNumPctKeys, m_uiLastPctIdx, ucPctSize);

    unsigned int ui1, ui2;
    float fNormTime;
    GetLoc(fPct, ui1, ui2, fNormTime);

    if (GetFollow())
    {
        NiMatrix3 kRefFrame;
        CalcRefFrame(ui1, ui2, fNormTime, kRefFrame);
        m_kRotation.FromRotation(kRefFrame);
    }

    m_kPosition = GenPathInterp(ui1, ui2, fNormTime);
    kValue.SetTranslate(m_kPosition);
    if (GetFollow())
        kValue.SetRotate(m_kRotation);
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiPathInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::GetPercentLoc(float fPercent, unsigned int& uiI1,
    unsigned int& uiI2, float& fNormTime)
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    GetPathKeys(uiNumPathKeys, ePathType, ucSize);
    NIASSERT(uiNumPathKeys >= 2);

    if (fPercent == 0.0f)
    {
        uiI1 = 0;
        uiI2 = 1;
        fNormTime = 0.0f;
        return;
    }
    else if (fPercent < 0.0f)
    {
        fPercent = NiFmod(fPercent, 1.0f) + 1.0f;
    }

    if (fPercent == 1.0f)
    {
        uiI1 = uiNumPathKeys - 2;
        uiI2 = uiNumPathKeys - 1;
        fNormTime = 1.0f;
        return;
    }
    else if (fPercent > 1.0f)
    {
        fPercent = NiFmod(fPercent, 1.0f);
    }

    // determine which polynomial segment corresponds to fPercent
    float fLength = fPercent*m_fTotalLength;
    float fDist = 0.0f;
    float fTime = 0.0f;
    unsigned int ui0, ui1;
    for (ui0 = 0, ui1 = 1; ui0 < uiNumPathKeys-1; ui0++, ui1++)
    {
        if (fLength <= m_pfPartialLength[ui1])
        {
            // distance along segment
            fDist = fLength-m_pfPartialLength[ui0];
            
            // initial guess for integral upper limit
            fTime = fDist/(m_pfPartialLength[ui1]-m_pfPartialLength[ui0]);
            break;
        }
    }

    // use Newton's method to invert the path length integral
    const int iMaxIterations = 32;
    for (int i = 0; i < iMaxIterations; i++)
    {
        float fDiff = PathLength(ui0, ui1, fTime) - fDist;
        const float fTolerance = 1e-04f;
        if (NiAbs(fDiff) <= fTolerance)
            break;

        // assert: PathSpeed(i0,i1,fTime) != 0
        fTime -= fDiff/PathSpeed(ui0, ui1, fTime);
    }

    uiI1 = ui0;
    uiI2 = ui1;
    fNormTime = fTime;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::GetLoc(float fTime, unsigned int& uiI1,
    unsigned int& uiI2, float& fNormTime)
{
    if (GetConstVelocity())
    {
        GetPercentLoc(fTime, uiI1, uiI2, fNormTime);
        m_uiLastPathIdx = uiI1;
        return;
    }

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucSize);
    NIASSERT(uiNumPathKeys >= 1);

    // clamp to time interval of key sequence
    if (fTime <= pkPathKeys->GetKeyAt(0,ucSize)->GetTime())
    {
        uiI1 = 0;
        uiI2 = 1;
        fNormTime = 0.0f;
        return;
    }

    NIASSERT(uiNumPathKeys >= 2);
    if (fTime >= pkPathKeys->GetKeyAt(uiNumPathKeys - 1,ucSize)->GetTime())
    {
        uiI1 = uiNumPathKeys - 2;
        uiI2 = uiNumPathKeys - 1;
        fNormTime = 1.0f;
        return;
    }

    bool bFound = false;
    bool bFirstLoop = true;
    unsigned int ui1;
    unsigned int ui2 = 0;
    for (ui1 = m_uiLastPathIdx; ui1 < uiNumPathKeys - 1;)
    {
        //
        // Check for direct hit
        //
        if (fTime == pkPathKeys->GetKeyAt(ui1,ucSize)->GetTime())
        {
            m_uiLastPathIdx = ui1;
            ui2 = ui1 + 1;
            bFound = true;
            break;
        }
        else if (fTime > pkPathKeys->GetKeyAt(ui1,ucSize)->GetTime() &&
            fTime < pkPathKeys->GetKeyAt(ui1 + 1,ucSize)->GetTime())
        {
            m_uiLastPathIdx = ui1;
            ui2 = ui1 + 1;
            bFound = true;
            break;
        }
        if (ui1 == uiNumPathKeys - 2 && bFirstLoop)
        {
            ui1 = 0;
            bFirstLoop = false;
            continue;
        }

        ui1++;
    }
        
    NIASSERT(bFound);
        
    // Normalize the time between 0 and 1 for the time values represented
    // in the two keys that will be used as interpolants.
    fNormTime = (fTime - pkPathKeys->GetKeyAt(ui1,ucSize)->GetTime()) / 
        (pkPathKeys->GetKeyAt(ui2,ucSize)->GetTime() -
        pkPathKeys->GetKeyAt(ui1,ucSize)->GetTime());
    uiI1 = ui1;
    uiI2 = ui2;
}
//---------------------------------------------------------------------------
NiPoint3 NiPathInterpolator::GenPathInterp(int iI1, int iI2, float fNormTime)
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucSize);

    NiPosKey::InterpFunction pfnInterp = 
        NiPosKey::GetInterpFunction(ePathType);
    NIASSERT(pfnInterp);
    NiPoint3 kResult;
    pfnInterp(fNormTime, pkPathKeys->GetKeyAt(iI1, ucSize), 
        pkPathKeys->GetKeyAt(iI2, ucSize), &kResult);
    return kResult;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::CalcRefFrame(unsigned int ui1, unsigned int ui2,
    float fNormTime, NiMatrix3& kRefFrame)
{
    NiMatrix3 kTm = NiMatrix3::IDENTITY;

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucSize);

    if (uiNumPathKeys < 2)
    {
        kRefFrame = kTm;
        return;
    }
    
    NiPoint3 kTangent, kNormal, kBinormal;
    float fCurvature;

    if (GetAllowFlip())
    {
        // calculate coordinate frame as kNormal
        NiPosKey::CoordinateFrame(fNormTime, pkPathKeys->GetKeyAt(ui1, 
            ucSize), pkPathKeys->GetKeyAt(ui2, ucSize), ePathType, kTangent,
            kNormal, kBinormal, fCurvature);

        // This code checks to see if time is within abs(tracking value).
        // If it is, the kNormal is calculated by finding two normals, 
        // one on each side of the control point in question, and weighting
        // their combination to give a single kNormal.  The purpose is to
        // give a result that will smooth any discontinuities in the
        // second derivative that might cause jumping/jerking as a 
        // controller passes over a control point.
        NiPoint3 kTmpTan, kNormal1;
        float fA0, fA1, fCurvature1;

        if (m_fSmoothing)
        {
            if (fNormTime >= 1.0f - m_fSmoothing)
            {
                unsigned int ui2a =
                  (!GetCurveTypeOpen() && ui2 == uiNumPathKeys-1 ? 0 : ui2);
                if (ui2a < uiNumPathKeys-1)
                {
                    fA0 = (1.0f - fNormTime + m_fSmoothing) /
                        (2.0f*m_fSmoothing);
                    fA1 = 1.0f - fA0;

                    NiPosKey::CoordinateFrame(1.0f - m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1, ucSize), 
                        pkPathKeys->GetKeyAt(ui2, ucSize),
                        ePathType, kTmpTan, kNormal, kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui2a, ucSize),
                        pkPathKeys->GetKeyAt(ui2a + 1, ucSize),
                        ePathType, kTmpTan, kNormal1, kBinormal, fCurvature1);

                    kNormal = fA0*kNormal + fA1*kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0*fCurvature + fA1*fCurvature1;
                    kBinormal = kTangent.Cross(kNormal);
                }
            }
            else if (fNormTime <= m_fSmoothing)
            {
                unsigned int ui1a =
                  (!GetCurveTypeOpen() && ui1 == 0 ? uiNumPathKeys-1 : ui1);
                if (ui1a >= 1)
                {
                    fA1 = (m_fSmoothing - fNormTime)/(2.0f*m_fSmoothing);
                    fA0 = 1.0f - fA1;

                    NiPosKey::CoordinateFrame(m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1, ucSize), 
                        pkPathKeys->GetKeyAt(ui2, ucSize),
                        ePathType, kTmpTan, kNormal, kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(1.0f - m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1a - 1, ucSize),
                        pkPathKeys->GetKeyAt(ui1a, ucSize),ePathType,
                        kTmpTan, kNormal1, kBinormal, fCurvature1);
                    kNormal = fA0*kNormal + fA1*kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0*fCurvature + fA1*fCurvature1;
                    kBinormal = kTangent.Cross(kNormal);
                }
            }
        }
        
        // direction
        kTm.SetCol(0, kTangent);
    
        // up
        kTm.SetCol(1, -kNormal);
        
        // right
        kTm.SetCol(2, -kBinormal);
    }
    else
    {
        // calculate the coordinate frame
        NiPosKey::CoordinateFrame(fNormTime, pkPathKeys->GetKeyAt(ui1, 
            ucSize), pkPathKeys->GetKeyAt(ui2, ucSize), ePathType, kTangent,
            kNormal, kBinormal, fCurvature);

        // direction
        kTm.SetCol(0, kTangent);

        // up
        NiPoint3 up = NiPoint3::UNIT_Z.UnitCross(kTangent);
        kTm.SetCol(1, up);

        // right
        NiPoint3 right = kTangent.Cross(up);
        kTm.SetCol(2, right);
    }

    // swap axes if necessary
    if (GetFlip())
    {
        NiPoint3 kTemp;
        // at
        kTm.GetCol(0, kTemp);

        kTm.SetCol(0, -kTemp);

        // up
        kTm.GetCol(1, kTemp);

        kTm.SetCol(1, -kTemp);
    }

    NiPoint3 kTmp, kTmp2;

    switch (m_sFollowAxis)
    {
        case 1:
            kTm.GetCol(0, kTmp);
            kTm.GetCol(1, kTmp2);
            kTm.SetCol(0, -kTmp2);
            kTm.SetCol(1, kTmp);
            break;
        case 2:
            kTm.GetCol(0, kTmp);
            kTm.GetCol(2, kTmp2);
            kTm.SetCol(0, -kTmp2);
            kTm.SetCol(2, kTmp);
            break;
    } 

    if (GetBank())
    {
        static float s_fFourOverPi = 4.0f/NI_PI;
        float fBankAmt = float(m_eBankDir)*m_fMaxBankAngle;
        if (fCurvature < m_fMaxCurvature)
            fBankAmt *= s_fFourOverPi*NiATan(fCurvature/m_fMaxCurvature);

        NiMatrix3 kBankMat;
        
        kBankMat.MakeRotation(fBankAmt, kTangent);
        kTm = kBankMat*kTm;
    }

    kRefFrame = kTm;
}
//---------------------------------------------------------------------------
float NiPathInterpolator::PathSpeed(int iI0, int iI1, float fTime) const
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucSize);

    NiPosKey::InterpFunction interpD1 =
        NiPosKey::GetInterpD1Function(ePathType);
    NIASSERT(interpD1);

    NiPoint3 deriv;
    interpD1(fTime, pkPathKeys->GetKeyAt(iI0, ucSize), 
        pkPathKeys->GetKeyAt(iI1, ucSize), &deriv);
    return deriv.Length();
}
//---------------------------------------------------------------------------
float NiPathInterpolator::PathLength(int iI0, int iI1, float fTime) const
{
    // Legendre polynomial information for Gaussian quadrature of speed
    // on domain [0,fTime], 0 <= fTime <= 1.
    const int iDegree = 5;
    static float s_afModRoot[iDegree] =
    {
        // Legendre roots mapped to (root+1)/2
        0.046910077f,
        0.230765345f,
        0.5f,
        0.769234655f,
        0.953089922f
    };
    static float s_afModCoeff[iDegree] =
    {
        // original coefficients divided by 2
        0.118463442f,
        0.239314335f,
        0.284444444f,
        0.239314335f,
        0.118463442f
    };

    // Need to transform domain [0,fTime] to [-1,1].  If 0 <= x <= fTime
    // and -1 <= t <= 1, then x = fTime*(t+1)/2.
    float fResult = 0.0f;
    for (int i = 0; i < iDegree; i++)
        fResult += s_afModCoeff[i]*PathSpeed(iI0,iI1,fTime*s_afModRoot[i]);
    fResult *= fTime;

    return fResult;
}
//---------------------------------------------------------------------------
bool NiPathInterpolator::GetChannelPosed(unsigned short) const
{
    return false;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;

    // Only get range for keys at channel 1 (the percent keys). The key times
    // for the path data must have keys that are timed from 0.0f to 1.0f and
    // do not correspond to actual time.
    unsigned int uiChannel = 1;
    if (GetKeyCount((unsigned char)uiChannel) > 0)
    {
        NiAnimationKey* pkBeginKey = GetKeyAt(0, (unsigned char)uiChannel);
        NiAnimationKey* pkEndKey = 
            GetKeyAt(GetKeyCount((unsigned char)uiChannel) - 1,
            (unsigned char)uiChannel);
        if(pkBeginKey && pkEndKey)
        {
            if (pkBeginKey->GetTime() < fBeginKeyTime)
                fBeginKeyTime = pkBeginKey->GetTime();
            if (pkEndKey->GetTime() > fEndKeyTime)
                fEndKeyTime = pkEndKey->GetTime();
        }
    }

    if (fBeginKeyTime == NI_INFINITY && fEndKeyTime == -NI_INFINITY)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiPathInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    // m_spPathData must have keys that are timed from 0.0f to 1.0f, so
    // it should NOT have GuaranteeKeysAtStartAndEnd called on it.

    if (m_spPctData)
    {
        m_spPctData->GuaranteeKeysAtStartAndEnd(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiPathInterpolator::GetSequenceInterpolator(float fStartTime,
    float fEndTime)
{
    NiPathInterpolator* pkSeqInterp = (NiPathInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spPctData)
    {
        NiFloatDataPtr spNewPctData = m_spPctData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqInterp->SetPctData(spNewPctData);
        pkSeqInterp->m_uiLastPctIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPathInterpolator);
//---------------------------------------------------------------------------
void NiPathInterpolator::CopyMembers(NiPathInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;

    // NOTE.  By calling CreateClone() rather than passing m_spPathData &
    // m_spPctData directly, this allows any change in what "clone" means to
    // be encapsulated by the CreateClone() call and does not require 
    // changing the code here.
    if (m_spPathData)
    {
        pkDest->SetPathData((NiPosData*)m_spPathData->CreateClone(
            kCloning));
    }

    if (m_spPctData)
    {
        pkDest->SetPctData((NiFloatData*)m_spPctData->CreateClone(
            kCloning));
    }

    pkDest->SetAllowFlip(GetAllowFlip());
    pkDest->SetBank(GetBank());
    pkDest->SetBankDir(GetBankDir());
    pkDest->SetConstVelocity(GetConstVelocity());
    pkDest->SetFollow(GetFollow());
    pkDest->SetMaxBankAngle(GetMaxBankAngle());
    pkDest->SetSmoothing(GetSmoothing());
    pkDest->SetFollowAxis(GetFollowAxis());
    pkDest->SetFlip(GetFlip());
    pkDest->SetCurveTypeOpen(GetCurveTypeOpen());

    if (m_pfPartialLength != NULL)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        unsigned char ucSize;
        GetPathKeys(uiNumPathKeys, ePathType, ucSize);
        NIASSERT(uiNumPathKeys > 0);
        pkDest->m_pfPartialLength = NiAlloc(float,uiNumPathKeys);
        NIASSERT(pkDest->m_pfPartialLength != NULL);
        unsigned int uiDestSize = uiNumPathKeys * sizeof(float);
        NiMemcpy(pkDest->m_pfPartialLength, uiDestSize, m_pfPartialLength,
            sizeof(m_pfPartialLength[0]) * uiNumPathKeys);
    }

    pkDest->m_fTotalLength = m_fTotalLength;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::ProcessClone(NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::ProcessClone(kCloning);

    if (m_spPathData)
    {
        m_spPathData->ProcessClone(kCloning);
    }

    if (m_spPctData)
    {
        m_spPctData->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPathInterpolator);
//---------------------------------------------------------------------------
void NiPathInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadEnum(kStream, m_eBankDir);
    NiStreamLoadBinary(kStream, m_fMaxBankAngle);
    NiStreamLoadBinary(kStream, m_fSmoothing);
    NiStreamLoadBinary(kStream, m_sFollowAxis);

    kStream.ReadLinkID();   // m_spPathData
    kStream.ReadLinkID();   // m_spPctData
}
//---------------------------------------------------------------------------
void NiPathInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);

    m_spPathData = (NiPosData*)kStream.GetObjectFromLinkID();

    m_spPctData = (NiFloatData*)kStream.GetObjectFromLinkID();

    // Update constant velocity data.
    SetCVDataNeedsUpdate(true);
    if (GetConstVelocity())
        SetConstantVelocityData();

    // approximate the maximum curvature of path
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    unsigned char ucSize;
    NiPosKey* pkPathKeys = GetPathKeys(uiNumPathKeys, ePathType, ucSize);
    if (uiNumPathKeys > 0)
    {
        NiPosKey::CurvatureFunction pfnCF;
        pfnCF = NiPosKey::GetCurvatureFunction(ePathType);
        m_fMaxCurvature = pfnCF((NiAnimationKey*) pkPathKeys, uiNumPathKeys);
    }
}
//---------------------------------------------------------------------------
bool NiPathInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
        return false;
    
    if (m_spPathData)
        m_spPathData->RegisterStreamables(kStream);

    if (m_spPctData)
        m_spPctData->RegisterStreamables(kStream);
    return true;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    NiStreamSaveEnum(kStream, m_eBankDir);
    NiStreamSaveBinary(kStream, m_fMaxBankAngle);
    NiStreamSaveBinary(kStream, m_fSmoothing);
    NiStreamSaveBinary(kStream, m_sFollowAxis);

    kStream.SaveLinkID(m_spPathData);
    kStream.SaveLinkID(m_spPctData);
}
//---------------------------------------------------------------------------
bool NiPathInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
        return false;

    NiPathInterpolator* pkDest = (NiPathInterpolator*) pkObject;

    if ((m_spPathData && !pkDest->m_spPathData) ||
        (!m_spPathData && pkDest->m_spPathData) ||
        (m_spPathData && !m_spPathData->IsEqual(pkDest->m_spPathData)))
    {
        return false;
    }

    if ((m_spPctData && !pkDest->m_spPctData) ||
        (!m_spPctData && pkDest->m_spPctData) ||
        (m_spPctData && !m_spPctData->IsEqual(pkDest->m_spPctData)))
    {
        return false;
    }

    if ((m_pfPartialLength && !pkDest->m_pfPartialLength)
    ||   (!m_pfPartialLength && pkDest->m_pfPartialLength))
    {
        return false;
    }

    if (m_pfPartialLength)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        unsigned char ucPathSize;
        GetPathKeys(uiNumPathKeys, ePathType, ucPathSize);
        
        unsigned int uiCount = uiNumPathKeys*sizeof(float);
        if (memcmp(m_pfPartialLength,pkDest->m_pfPartialLength,uiCount) != 0)
            return false;
    }

    if (m_fTotalLength != pkDest->m_fTotalLength ||
        GetCVDataNeedsUpdate() != pkDest->GetCVDataNeedsUpdate() ||
        GetCurveTypeOpen() != pkDest->GetCurveTypeOpen() ||
        m_eBankDir != pkDest->m_eBankDir ||
        GetAllowFlip() != pkDest->GetAllowFlip() ||
        GetBank() != pkDest->GetBank() ||
        GetConstVelocity() != pkDest->GetConstVelocity() ||
        GetFollow() != pkDest->GetFollow() ||
        m_fMaxBankAngle != pkDest->m_fMaxBankAngle ||
        m_fSmoothing != pkDest->m_fSmoothing ||
        m_sFollowAxis != pkDest->m_sFollowAxis ||
        GetFlip() != pkDest->GetFlip())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPathInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPathInterpolator::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
