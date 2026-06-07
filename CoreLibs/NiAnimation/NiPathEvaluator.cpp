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

#include "NiPathEvaluator.h"
#include "NiQuatTransform.h"
#include "NiEvaluatorSPData.h"

NiImplementRTTI(NiPathEvaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
float NiPathEvaluator::PathLength(float fNorm, const NiPosKey* pkPosKey1, 
    const NiPosKey* pkPosKey2, NiPosKey::InterpFunction interpD1)
{
    NIASSERT(interpD1);

    // Legendre polynomial information for Gaussian quadrature of speed
    // on domain [0,fNorm], 0 <= fNorm <= 1.
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

    // Need to transform domain [0,fNorm] to [-1,1].  If 0 <= x <= fNorm
    // and -1 <= t <= 1, then x = fNorm*(t+1)/2.
    float fResult = 0.0f;
    for (int i = 0; i < iDegree; i++)
    {
        NiPoint3 deriv;
        interpD1(fNorm * s_afModRoot[i], pkPosKey1, pkPosKey2, &deriv);
        fResult += s_afModCoeff[i] * deriv.Length();
    }
    fResult *= fNorm;

    return fResult;
}
//---------------------------------------------------------------------------
void NiPathEvaluator::CalcRefFrame(
#ifdef NIDEBUG
    float fPct, 
#else
    float, 
#endif
    float fNorm, 
    const NiScratchPad::PathFillData* pkPathFill, NiMatrix3& kRefFrame)
{
    NiMatrix3 kTm = NiMatrix3::IDENTITY;

    const NiScratchPad::CubicPoint3FillData* pkPosFill = 
        &pkPathFill->m_kPosFill;
    if (pkPosFill->m_uiNumKeys < 2)
    {
        kRefFrame = kTm;
        return;
    }

    NiPosKey* pkPosKeys = pkPosFill->m_pkKeys;
    unsigned int uiNumPosKeysM1 = pkPosFill->m_uiNumKeys - 1;
    NiPosKey::KeyType ePosKeyType = pkPosFill->m_eType;
    unsigned char ucPosKeySize = pkPosFill->m_ucKeySize;

    NIASSERT(pkPosFill->m_pkKeys);
    unsigned int ui1 = pkPosFill->m_uiLastIndex;
    const NiPosKey* pkPosKey1 = pkPosKeys->GetKeyAt(ui1, 
        ucPosKeySize);
    NIASSERT(fPct >= pkPosKey1->GetTime());

    unsigned int ui2 = ui1 + 1;
    NIASSERT(ui2 <= uiNumPosKeysM1);
    const NiPosKey* pkPosKey2 = pkPosKeys->GetKeyAt(ui2, 
        ucPosKeySize);
    NIASSERT(fPct <= pkPosKey2->GetTime());

    NiPoint3 kTangent, kNormal, kBinormal;
    float fCurvature;

    // Calculate coordinate frame as kNormal.
    NiPosKey::CoordinateFrame(fNorm, pkPosKey1, pkPosKey2,
        ePosKeyType, kTangent, kNormal, kBinormal, fCurvature);

    bool bIsCurveTypeOpen = ((pkPathFill->m_usFlags & OPENCURVE_MASK) != 0);

    if ((pkPathFill->m_usFlags & ALLOWFLIP_MASK) != 0)
    {
        // This code checks to see if time is within abs(tracking value).
        // If it is, the kNormal is calculated by finding two normals, 
        // one on each side of the control point in question, and weighting
        // their combination to give a single kNormal.  The purpose is to
        // give a result that will smooth any discontinuities in the
        // second derivative that might cause jumping/jerking as a 
        // controller passes over a control point.
        NiPoint3 kTmpTan, kNormal1;
        float fA0, fA1, fCurvature1;

        if (pkPathFill->m_fSmoothing)
        {
            if (fNorm >= 1.0f - pkPathFill->m_fSmoothing)
            {
                unsigned int ui2a =
                  (!bIsCurveTypeOpen && ui2 == uiNumPosKeysM1 ? 0 : ui2);
                if (ui2a < uiNumPosKeysM1)
                {
                    fA0 = (1.0f - fNorm + pkPathFill->m_fSmoothing) /
                        (2.0f * pkPathFill->m_fSmoothing);
                    fA1 = 1.0f - fA0;

                    NiPosKey::CoordinateFrame(1.0f - pkPathFill->m_fSmoothing,
                        pkPosKey1, pkPosKey2, ePosKeyType, kTmpTan, kNormal, 
                        kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(pkPathFill->m_fSmoothing,
                        pkPosKeys->GetKeyAt(ui2a, ucPosKeySize),
                        pkPosKeys->GetKeyAt(ui2a + 1, ucPosKeySize), 
                        ePosKeyType, kTmpTan, kNormal1, kBinormal, 
                        fCurvature1);

                    kNormal = fA0 * kNormal + fA1 * kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0 * fCurvature + fA1 * fCurvature1;
                    kBinormal = kTangent.Cross(kNormal);
                }
            }
            else if (fNorm <= pkPathFill->m_fSmoothing)
            {
                unsigned int ui1a =
                  (!bIsCurveTypeOpen && ui1 == 0 ? uiNumPosKeysM1 : ui1);
                if (ui1a >= 1)
                {
                    fA1 = (pkPathFill->m_fSmoothing - fNorm) / 
                        (2.0f * pkPathFill->m_fSmoothing);
                    fA0 = 1.0f - fA1;

                    NiPosKey::CoordinateFrame(pkPathFill->m_fSmoothing,
                        pkPosKey1, pkPosKey2,
                        ePosKeyType, kTmpTan, kNormal, kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(1.0f - pkPathFill->m_fSmoothing,
                        pkPosKeys->GetKeyAt(ui1a - 1, ucPosKeySize),
                        pkPosKeys->GetKeyAt(ui1a, ucPosKeySize), ePosKeyType,
                        kTmpTan, kNormal1, kBinormal, fCurvature1);
                    kNormal = fA0 * kNormal + fA1 * kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0 * fCurvature + fA1 * fCurvature1;
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
    if ((pkPathFill->m_usFlags & FLIP_MASK) != 0)
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

    switch (pkPathFill->m_sFollowAxis)
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

    if ((pkPathFill->m_usFlags & BANK_MASK) != 0)
    {
        static float s_fFourOverPi = 4.0f / NI_PI;
        float fBankAmt = float(pkPathFill->m_iBankDir) * 
            pkPathFill->m_fMaxBankAngle;
        if (fCurvature < pkPathFill->m_fMaxCurvature)
        {
            fBankAmt *= s_fFourOverPi * 
                NiATan(fCurvature / pkPathFill->m_fMaxCurvature);
        }

        NiMatrix3 kBankMat;
        
        kBankMat.MakeRotation(fBankAmt, kTangent);
        kTm = kBankMat * kTm;
    }

    kRefFrame = kTm;
}
//---------------------------------------------------------------------------
NiPathEvaluator::NiPathEvaluator() :
    m_spPathData(NULL), 
    m_spPctData(NULL)
{
    m_uFlags = 0;

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

    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the other eval channel types.
}
//---------------------------------------------------------------------------
NiPathEvaluator::NiPathEvaluator(NiPosData* pkPathData,
    NiFloatData* pkPctData) :
    m_spPathData(pkPathData), 
    m_spPctData(pkPctData)
{
    m_uFlags = 0;

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

    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiPathEvaluator::~NiPathEvaluator()
{
    m_spPathData = 0;
    m_spPctData = 0;
    NiFree(m_pfPartialLength);
}
//---------------------------------------------------------------------------
unsigned short NiPathEvaluator::GetKeyChannelCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
unsigned int NiPathEvaluator::GetKeyCount(unsigned short usChannel) const
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
NiAnimationKey::KeyType NiPathEvaluator::GetKeyType(
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
NiAnimationKey::KeyContent NiPathEvaluator::GetKeyContent(
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
NiAnimationKey* NiPathEvaluator::GetKeyArray(unsigned short usChannel)
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
unsigned char NiPathEvaluator::GetKeyStride(unsigned short usChannel)
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
void NiPathEvaluator::ReplacePathKeys(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (!m_spPathData)
        m_spPathData = NiNew NiPosData;

    m_spPathData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    unsigned char ucSize = NiPosKey::GetKeySize(eType);

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

    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
void NiPathEvaluator::ReplacePctKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (!m_spPctData)
        m_spPctData = NiNew NiFloatData;

    m_spPctData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
float NiPathEvaluator::GetTotalPathLength() const
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
bool NiPathEvaluator::GetChannelPosedValue(
#ifdef NIDEBUG
    unsigned int uiChannel, void* pvResult) const
#else
    unsigned int, void*) const
#endif
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pvResult);

    // Path channels are never posed.
    NIASSERT(!IsRawEvalChannelPosed(uiChannel));

    return false;
}
//---------------------------------------------------------------------------
bool NiPathEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsEvalChannelInvalid(uiChannel))
    {
        return false;
    }

    // Fill the segment data, if stale.
    if (!pkEvalSPData->IsSPSegmentDataValid(fTime))
    {
        NiScratchPadFillFunc pfnFillFunc = pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    // Copy the computed position or rotation.
    if (uiChannel == POSITION)
    {
        NiScratchPad::CubicPoint3SegmentData* pkCubicPt3 = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicPt3);
        *(NiPoint3*)pvResult = pkCubicPt3->m_kValue0;
    }
    else
    {
        NIASSERT(uiChannel == ROTATION);
        NiScratchPad::SlerpRotSegmentData* pkSlerpRot = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpRot);
        *(NiQuaternion*)pvResult = pkSlerpRot->m_kQuat0;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPathEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = sizeof(NiScratchPad::PathFillData);
    if (m_pfPartialLength)
    {
        // Expand fill size to store the m_pfPartialLength array.
        uiFillSize += m_spPathData->GetNumKeys() * sizeof(float);
    }

    // Share the fill data with the position and rotation channels.
    bSharedFillData = true;

    if (uiChannel == POSITION)
    {
        eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
    }
    else
    {
        NIASSERT(uiChannel == ROTATION);
        eSPBSegmentData = SPBSLERPROTSEGMENT;
    }
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiPathEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool bInitSharedData, 
#ifdef NIDEBUG
    NiAVObjectPalette* pkPalette, 
#else
    NiAVObjectPalette*, 
#endif
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == POSITION || uiChannel == ROTATION)
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_spPathData && m_spPathData->GetNumKeys() > 0);
    NIASSERT(m_spPctData && m_spPctData->GetNumKeys() > 0);

    // Partially initialize the scratch pad segment data.
    NiScratchPad::SegmentDataHeader* pkSegHeader = 
        (NiScratchPad::SegmentDataHeader*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkSegHeader);

    pkSegHeader->m_sLOD = pkEvalSPData->GetLOD();
    if (IsReferencedEvaluator())
    {
        NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
        pkSegHeader->m_usOutputIndex = NiScratchPad::INVALIDOUTPUTINDEX;
    }
    else
    {
        NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL ||
            kPBHandle.GetChannelType() == PBROTCHANNEL);
        pkSegHeader->m_usOutputIndex = kPBHandle.GetChannelIndex();
    }

    NiScratchPad::CubicPoint3SegmentData* pkPosSeg = NULL;
    NiScratchPad::SlerpRotSegmentData* pkRotSeg = NULL;
    if (uiChannel == POSITION)
    {
        pkPosSeg = (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkPosSeg->m_fStartTime = NI_INFINITY;
        pkPosSeg->m_fInvDeltaTime = 0.0f;
        pkPosSeg->m_kValue0 = NiPoint3::ZERO;
        pkPosSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkPosSeg->m_kA0 = NiPoint3::ZERO;
        pkPosSeg->m_kB0 = NiPoint3::ZERO;
#endif
    }
    else
    {
        pkRotSeg = (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkRotSeg->m_fStartTime = NI_INFINITY;
        pkRotSeg->m_fInvDeltaTime = 0.0f;
        pkRotSeg->m_kQuat0 = NiQuaternion::IDENTITY;
        pkRotSeg->m_kQuat1 = NiQuaternion::IDENTITY;
#endif
    }

    if (bInitSharedData)
    {
        // Initialize the scratch pad fill data.
        NiScratchPad::PathFillData* pkFillData = 
            (NiScratchPad::PathFillData*)pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);

        pkFillData->m_fLastTime = -NI_INFINITY;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkFillData->m_kPosition = NiPoint3::ZERO;
        pkFillData->m_kRotation = NiQuaternion::IDENTITY;
#endif
        pkFillData->m_fPctMinTime = NI_INFINITY;
        pkFillData->m_fPctMaxTime = -NI_INFINITY;

        pkFillData->m_kPctFill.m_uiLastIndex = 0;
        pkFillData->m_kPctFill.m_pkKeys = m_spPctData->GetAnim(
            pkFillData->m_kPctFill.m_uiNumKeys, 
            pkFillData->m_kPctFill.m_eType, 
            pkFillData->m_kPctFill.m_ucKeySize);

        pkFillData->m_kPctSeg.m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
        pkFillData->m_kPctSeg.m_kHeader.m_usOutputIndex = 
            NiScratchPad::INVALIDOUTPUTINDEX;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkFillData->m_kPctSeg.m_fStartTime = NI_INFINITY;
        pkFillData->m_kPctSeg.m_fInvDeltaTime = 0.0f;
        pkFillData->m_kPctSeg.m_fValue0 = 0.0f;
        pkFillData->m_kPctSeg.m_fOutTangent0 = 0.0f;
        pkFillData->m_kPctSeg.m_fA0 = 0.0f;
        pkFillData->m_kPctSeg.m_fB0 = 0.0f;
#endif

        pkFillData->m_fPosMinPct = NI_INFINITY;
        pkFillData->m_fPosMaxPct = -NI_INFINITY;

        pkFillData->m_kPosFill.m_uiLastIndex = 0;
        pkFillData->m_kPosFill.m_pkKeys = m_spPathData->GetAnim(
            pkFillData->m_kPosFill.m_uiNumKeys, 
            pkFillData->m_kPosFill.m_eType, 
            pkFillData->m_kPosFill.m_ucKeySize);

        pkFillData->m_kPosSeg.m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
        pkFillData->m_kPosSeg.m_kHeader.m_usOutputIndex = 
            NiScratchPad::INVALIDOUTPUTINDEX;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkFillData->m_kPosSeg.m_fStartTime = NI_INFINITY;
        pkFillData->m_kPosSeg.m_fInvDeltaTime = 0.0f;
        pkFillData->m_kPosSeg.m_kValue0 = NiPoint3::ZERO;
        pkFillData->m_kPosSeg.m_kOutTangent0 = NiPoint3::ZERO;
        pkFillData->m_kPosSeg.m_kA0 = NiPoint3::ZERO;
        pkFillData->m_kPosSeg.m_kB0 = NiPoint3::ZERO;
#endif

        pkFillData->m_usFlags = m_uFlags;
        pkFillData->m_sFollowAxis = m_sFollowAxis;
        pkFillData->m_fMaxBankAngle = m_fMaxBankAngle;
        pkFillData->m_fSmoothing = m_fSmoothing;
        pkFillData->m_fMaxCurvature = m_fMaxCurvature;
        pkFillData->m_iBankDir = m_eBankDir;
        pkFillData->m_fTotalLength = m_fTotalLength;
        if (GetConstVelocity())
        {
            // Partial length data begins immediately after m_pfPartialLength.
            pkFillData->m_pfPartialLength = (float*)((char*)pkFillData + 
                sizeof(NiScratchPad::PathFillData));
            unsigned int uiNumLengths = pkFillData->m_kPosFill.m_uiNumKeys;
#ifdef NIDEBUG
            unsigned int uiFillSize;
            bool bSharedFillData;
            NiScratchPadBlock eSPBSegmentData;
            NiBSplineBasisData* pkBasisData;
            NIVERIFY(GetChannelScratchPadInfo(uiChannel, true, pkPalette, 
                uiFillSize, bSharedFillData, eSPBSegmentData, pkBasisData));
            NIASSERT((unsigned int)
                ((char*)&pkFillData->m_pfPartialLength[uiNumLengths] - 
                (char*)pkFillData) == uiFillSize);
#endif
            for (unsigned int ui = 0; ui < uiNumLengths; ui++)
            {
                pkFillData->m_pfPartialLength[ui] = m_pfPartialLength[ui];
            }
        }
        else
        {
            pkFillData->m_pfPartialLength = NULL;
        }
    }

    NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
    pkEvalSPData->SetSPFillFunc(&PathFillFunction);

    return true;
}
//---------------------------------------------------------------------------
bool NiPathEvaluator::PathFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::PathFillData* pkPathFill = 
        (NiScratchPad::PathFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkPathFill);

    // Compute new path values when the time doesn't match the prior time.
    if (fTime != pkPathFill->m_fLastTime)
    {
        // Fill the percent segment data, if stale.
        NiScratchPad::CubicFloatSegmentData* pkPctSeg = 
            &pkPathFill->m_kPctSeg;
        if (fTime > pkPathFill->m_fPctMaxTime || 
            fTime < pkPathFill->m_fPctMinTime)
        {
            NiScratchPad::CubicFloatFillData* pkPctFill = 
                &pkPathFill->m_kPctFill;
            NIASSERT(pkPctFill->m_pkKeys);
            NIASSERT(pkPctFill->m_uiNumKeys > 0);
            NiFloatKey::GenCubicCoefs(fTime, pkPctFill->m_pkKeys, 
                pkPctFill->m_eType, pkPctFill->m_uiNumKeys, 
                pkPctFill->m_uiLastIndex, pkPctFill->m_ucKeySize, 
                pkPathFill->m_fPctMinTime, pkPathFill->m_fPctMaxTime, 
                pkPctSeg->m_fValue0, pkPctSeg->m_fOutTangent0, 
                pkPctSeg->m_fA0, pkPctSeg->m_fB0);
            pkPctSeg->m_fStartTime = pkPathFill->m_fPctMinTime;
            pkPctSeg->m_fInvDeltaTime = 
                (pkPathFill->m_fPctMaxTime != pkPathFill->m_fPctMinTime) ? 
                1.0f / (pkPathFill->m_fPctMaxTime - pkPathFill->m_fPctMinTime)
                : 0.0f;
        }

        // Compute the percent completed based on the percent segment data.
        float fT = (fTime - pkPctSeg->m_fStartTime) * 
            pkPctSeg->m_fInvDeltaTime;
        float fPct = pkPctSeg->m_fValue0 + (pkPctSeg->m_fOutTangent0 + 
            (pkPctSeg->m_fA0 + pkPctSeg->m_fB0 * fT) * fT) * fT;

        // Modify the percent completed based on arc length, if requested.
        NiScratchPad::CubicPoint3FillData* pkPosFill = 
            &pkPathFill->m_kPosFill;
        NIASSERT(pkPosFill->m_pkKeys);
        NIASSERT(pkPosFill->m_uiNumKeys > 0);
        if ((pkPathFill->m_usFlags & CONSTVELOCITY_MASK) != 0)
        {
            if (fPct < 0.0f)
            {
                fPct = NiFmod(fPct, 1.0f) + 1.0f;
            }
            else if (fPct > 1.0f)
            {
                fPct = NiFmod(fPct, 1.0f);
            }

            float fNewPct = fPct;
            if (fPct > 0.0f && fPct < 1.0f)
            {
                // Determine which polynomial segment corresponds to fPct.
                float fLength = fPct * pkPathFill->m_fTotalLength;
                float fDist = 0.0f;
                float fNorm = 0.0f;
                float* pfPartialLength = pkPathFill->m_pfPartialLength;
                unsigned int uiNumPosKeysM1 = pkPosFill->m_uiNumKeys - 1;
                unsigned int ui0, ui1;
                for (ui0 = 0, ui1 = 1; ui0 < uiNumPosKeysM1; ui0++, ui1++)
                {
                    if (fLength <= pfPartialLength[ui1])
                    {
                        // Distance along segment.
                        fDist = fLength - pfPartialLength[ui0];
                        
                        // Initial guess for integral upper limit.
                        fNorm = fDist / 
                            (pfPartialLength[ui1] - pfPartialLength[ui0]);
                        break;
                    }
                }

                // Obtain the bounding keys and first derivative function.
                const NiPosKey* pkPosKey1 = pkPosFill->m_pkKeys->GetKeyAt(
                    ui0, pkPosFill->m_ucKeySize);
                const NiPosKey* pkPosKey2 = pkPosFill->m_pkKeys->GetKeyAt(
                    ui1, pkPosFill->m_ucKeySize);
                NiPosKey::InterpFunction interpD1 =
                    NiPosKey::GetInterpD1Function(pkPosFill->m_eType);
                NIASSERT(interpD1);

                // Use Newton's method to invert the path length integral.
                const int iMaxIterations = 32;
                for (int i = 0; i < iMaxIterations; i++)
                {
                    float fDiff = PathLength(fNorm, pkPosKey1, 
                        pkPosKey2, interpD1) - fDist;
                    const float fTolerance = 1e-04f;
                    if (NiAbs(fDiff) <= fTolerance)
                        break;

                    // Adjust for the speed along the path.
                    NiPoint3 deriv;
                    interpD1(fNorm, pkPosKey1, pkPosKey2, &deriv);
                    // assert: deriv.Length() != 0
                    fNorm -= fDiff / deriv.Length();
                }

                // Calculate the new percentage along the path.
                fNewPct = pkPosKey1->GetTime() + 
                    fNorm * (pkPosKey2->GetTime() - pkPosKey1->GetTime());
            }

            // Use the newly computed percentage.
            fPct = fNewPct;
        }

        // Map the percentage to a range of 0 to 1.
        if (fPct < 0.0f)
        {
            fPct = NiFmod(fPct, 1.0f) + 1.0f;
        }
        else if (fPct > 1.0f)
        {
            fPct = NiFmod(fPct, 1.0f);
        }

        // Fill the position segment data, if stale.
        NiScratchPad::CubicPoint3SegmentData* pkPosSeg = 
            &pkPathFill->m_kPosSeg;
        if (fPct > pkPathFill->m_fPosMaxPct || 
            fPct < pkPathFill->m_fPosMinPct)
        {
            NiPosKey::GenCubicCoefs(fPct, pkPosFill->m_pkKeys, 
                pkPosFill->m_eType, pkPosFill->m_uiNumKeys, 
                pkPosFill->m_uiLastIndex, pkPosFill->m_ucKeySize, 
                pkPathFill->m_fPosMinPct, pkPathFill->m_fPosMaxPct, 
                pkPosSeg->m_kValue0, pkPosSeg->m_kOutTangent0, 
                pkPosSeg->m_kA0, pkPosSeg->m_kB0);
            pkPosSeg->m_fStartTime = pkPathFill->m_fPosMinPct;
            pkPosSeg->m_fInvDeltaTime = 
                (pkPathFill->m_fPosMaxPct != pkPathFill->m_fPosMinPct) ? 
                1.0f / (pkPathFill->m_fPosMaxPct - pkPathFill->m_fPosMinPct) :
                0.0f;
        }

        // Compute the position based on the position segment data.
        fT = (fPct - pkPosSeg->m_fStartTime) * pkPosSeg->m_fInvDeltaTime;
        pkPathFill->m_kPosition = pkPosSeg->m_kValue0 + 
            (pkPosSeg->m_kOutTangent0 + (pkPosSeg->m_kA0 + 
            pkPosSeg->m_kB0 * fT) * fT) * fT;

        // Compute the rotation when the follow flag is set.
        if ((pkPathFill->m_usFlags & FOLLOW_MASK) != 0)
        {
            NiMatrix3 kRefFrame;
            CalcRefFrame(fPct, fT, pkPathFill, kRefFrame);
            pkPathFill->m_kRotation.FromRotation(kRefFrame);
        }

        pkPathFill->m_fLastTime = fTime;
    }

    // Copy the computed position or rotation to the associated
    // output segment data.
    if ((EvalChannel)pkEvalSPData->GetEvalChannelIndex() == POSITION)
    {
        NiScratchPad::CubicPoint3SegmentData* pkCubicPt3 = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicPt3);
        pkCubicPt3->m_kValue0 = pkPathFill->m_kPosition;

        // There's no need to fill the position scratch pad 
        // at an identical time.
        pkEvalSPData->SetSPSegmentTimeRange(fTime, fTime);
        return true;
    }

    if ((EvalChannel)pkEvalSPData->GetEvalChannelIndex() == ROTATION)
    {
        NiScratchPad::SlerpRotSegmentData* pkSlerpRot = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpRot);
        pkSlerpRot->m_kQuat0 = pkPathFill->m_kRotation;
        pkSlerpRot->m_kQuat1 = pkPathFill->m_kRotation;

        // There's no need to fill the rotation scratch pad 
        // at an identical time.
        pkEvalSPData->SetSPSegmentTimeRange(fTime, fTime);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiPathEvaluator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;

    // Only get range for keys at channel 1 (the percent keys). The key times
    // for the path data must have keys that are timed from 0.0f to 1.0f and
    // do not correspond to actual time.
    unsigned short usChannel = (unsigned short)PERCENTAGE;
    if (GetKeyCount(usChannel) > 0)
    {
        NiAnimationKey* pkBeginKey = GetKeyAt(0, usChannel);
        NiAnimationKey* pkEndKey = GetKeyAt(GetKeyCount(usChannel) - 1,
            usChannel);
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
void NiPathEvaluator::GuaranteeTimeRange(float fStartTime, 
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
NiEvaluator* NiPathEvaluator::GetSequenceEvaluator(float fStartTime,
    float fEndTime)
{
    NiPathEvaluator* pkSeqEval = (NiPathEvaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);
    if (m_spPctData)
    {
        NiFloatDataPtr spNewPctData = m_spPctData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqEval->SetPctData(spNewPctData);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiPathEvaluator::SetEvalChannelTypes()
{
    // Assume all three channels are invalid.
    m_aiEvalChannelTypes[POSITION] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[SCALE] = EVALINVALIDCHANNEL;

    if (m_spPathData && m_spPathData->GetNumKeys() > 0 &&
        m_spPctData && m_spPctData->GetNumKeys() > 0)
    {
        // Assume the channel is not posed.
        m_aiEvalChannelTypes[POSITION] = EVALPOINT3CHANNEL;

        if (GetFollow())
        {
            // Assume the channel is not posed.
            m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL;
        }
    }
}
//---------------------------------------------------------------------------
void NiPathEvaluator::SetConstantVelocityData()
{
    if (!GetCVDataNeedsUpdate())
        return;

    m_fTotalLength = GetTotalPathLength();

    SetCVDataNeedsUpdate(false);
}
//---------------------------------------------------------------------------
float NiPathEvaluator::PathSpeed(int iI0, int iI1, float fTime) const
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
float NiPathEvaluator::PathLength(int iI0, int iI1, float fTime) const
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

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPathEvaluator);
//---------------------------------------------------------------------------
void NiPathEvaluator::CopyMembers(NiPathEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);
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
void NiPathEvaluator::ProcessClone(NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::ProcessClone(kCloning);

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
NiImplementCreateObject(NiPathEvaluator);
//---------------------------------------------------------------------------
void NiPathEvaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadEnum(kStream, m_eBankDir);
    NiStreamLoadBinary(kStream, m_fMaxBankAngle);
    NiStreamLoadBinary(kStream, m_fSmoothing);
    NiStreamLoadBinary(kStream, m_sFollowAxis);

    kStream.ReadLinkID();   // m_spPathData
    kStream.ReadLinkID();   // m_spPctData
}
//---------------------------------------------------------------------------
void NiPathEvaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);

    m_spPathData = (NiPosData*)kStream.GetObjectFromLinkID();

    m_spPctData = (NiFloatData*)kStream.GetObjectFromLinkID();

    // Update constant velocity data.
    SetCVDataNeedsUpdate(true);
    if (GetConstVelocity())
    {
        SetConstantVelocityData();
    }

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
bool NiPathEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    
    if (m_spPathData)
    {
        m_spPathData->RegisterStreamables(kStream);
    }

    if (m_spPctData)
    {
        m_spPctData->RegisterStreamables(kStream);
    }
    return true;
}
//---------------------------------------------------------------------------
void NiPathEvaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    NiStreamSaveEnum(kStream, m_eBankDir);
    NiStreamSaveBinary(kStream, m_fMaxBankAngle);
    NiStreamSaveBinary(kStream, m_fSmoothing);
    NiStreamSaveBinary(kStream, m_sFollowAxis);

    kStream.SaveLinkID(m_spPathData);
    kStream.SaveLinkID(m_spPctData);
}
//---------------------------------------------------------------------------
bool NiPathEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
        return false;

    NiPathEvaluator* pkDest = (NiPathEvaluator*) pkObject;

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
void NiPathEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPathEvaluator::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
