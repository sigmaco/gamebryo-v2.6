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

#include "MaxImmerse.h"
#include "NiMAXAnimationConverter.h"
#include <NiOptimizeAnimation.h>

// Temporarily disable "for" scope conformance option
#pragma conform(forScope, push, f, off)
#include <IIKSys.h>
#pragma conform(forScope, pop, f)

#include "NiMAXHierConverter.h"
#include "NiUtils.h"

bool NiMAXAnimationConverter::ms_bConvertCSAsKeyframe = false;

void *NiMAXAnimationConverter::ms_pUserData = NULL;

NiTPointerList<NiMAXAnimationConverter::LookAtInfo *>
    NiMAXAnimationConverter::ms_lookAtList;

NiTPointerList<NiMAXAnimationConverter::PosRotAnimInfo *> 
    NiMAXAnimationConverter::ms_PosRotAnimList;

//---------------------------------------------------------------------------
// ORT == Out of Range Type
void NiMAXAnimationConverter::SetORT(Control* pCont, 
    NiTimeController::CycleType& eType)
{
    CHECK_MEMORY();
    
    NiTimeController::CycleType cycType = GetORT(pCont);
    
    if (eType == NiTimeController::CLAMP)
        eType = cycType;

}
//---------------------------------------------------------------------------
// ORT == Out of Range Type
NiTimeController::CycleType NiMAXAnimationConverter::SetORT(Control* pCont, 
    NiTimeController* pkController)
{
    CHECK_MEMORY();
    
    NiTimeController::CycleType cycType = GetORT(pCont);
    
    if (pkController && pkController->GetCycleType() == 
        NiTimeController::CLAMP)
    {
        pkController->SetCycleType(cycType);
    }

    return cycType;
}
//---------------------------------------------------------------------------
NiTimeController::CycleType NiMAXAnimationConverter::GetORT(Control *pCont)
{
    if (pCont == NULL)
        return NiTimeController::CLAMP;

    pCont->EnableORTs(true);
    int iORTb = pCont->GetORT(ORT_BEFORE);
    int iORTa = pCont->GetORT(ORT_AFTER);
    
    NiTimeController::CycleType cycType = NiTimeController::CLAMP;
    if (iORTb == ORT_CONSTANT && iORTa == ORT_CONSTANT)
        cycType = NiTimeController::CLAMP;
    else if (iORTb == ORT_CYCLE && iORTa == ORT_CYCLE)
        cycType = NiTimeController::LOOP;
    else if (iORTb == ORT_LOOP && iORTa == ORT_LOOP)
        cycType = NiTimeController::LOOP;
    else if (iORTb == ORT_OSCILLATE && iORTa == ORT_OSCILLATE)
        cycType = NiTimeController::REVERSE;
    else if (iORTb == ORT_CYCLE || iORTa == ORT_CYCLE ||
             iORTb == ORT_LOOP || iORTa == ORT_LOOP)
        cycType = NiTimeController::LOOP;

    return cycType;
}
//---------------------------------------------------------------------------
//                  FLOAT ANIMATION
//---------------------------------------------------------------------------

NiInterpolator* NiMAXAnimationConverter::ConvertTCBFloat(IKeyControl* pIKeys)
{
    CHECK_MEMORY();
    int iNumKeys = pIKeys->GetNumKeys();

    NiTCBFloatKey* pKeys = NiNew NiTCBFloatKey[iNumKeys];

    if (pKeys == NULL)
        return NULL;

    for (int i = 0; i < iNumKeys; i++)
    {
        ITCBFloatKey tcb;
        pIKeys->GetKey(i, &tcb);

        pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        pKeys[i].SetValue(tcb.val);
        pKeys[i].SetTension(tcb.tens);
        pKeys[i].SetContinuity(tcb.cont);
        pKeys[i].SetBias(tcb.bias);
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiFloatKey::TCBKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertBezFloat(IKeyControl* pIKeys)
{
    CHECK_MEMORY();
    int iNumKeys = pIKeys->GetNumKeys();

    NiBezFloatKey* pKeys = NiNew NiBezFloatKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;
    
    int iKey = 0;
    int i = 0;
    for (; i < iNumKeys; i++)
    {
        IBezFloatKey bez;
        pIKeys->GetKey(i, &bez);

        // skip keys at very large negative times (< -5000 sec)
        if (bez.time > -2.4e7)
        {
            pKeys[iKey].SetTime(bez.time * SECONDSPERTICK);
            pKeys[iKey].SetValue(bez.val);
            pKeys[iKey].SetInTan(bez.intan);
            pKeys[iKey].SetOutTan(bez.outtan);
            iKey++;
        }
        else
        {
            char acString[512];
            NiSprintf(acString, 512, "Invalid rotation key found at time %d, "
                "skipping that key!", (unsigned int) bez.time);
            NILOGWARNING(acString);
        }
    }
    iNumKeys = iKey;

    // We need to patch up the tangent vectors to match our interpolation
    // MAX's tangents are specified in ticks between samples and both 
    // vectors point into the segment. We convert this to 0..1 tangents
    // and the out tangent pointing away from the segment.
    if (iNumKeys)
    {
        float fLastTime, fCurTime, fNextTime, fTimeDelIn, fTimeDelOut;
        float fIn, fOut;

        fLastTime = fCurTime = pKeys[0].GetTime() * TICKSPERSECOND;
        NiBezFloatKey* pCurKey = &pKeys[0];
        fTimeDelIn = fCurTime - fLastTime;

        for (i = 0; i < iNumKeys; i++)
        {
            NiBezFloatKey* pNextKey;

            if (i+1 < iNumKeys)
            {
                pNextKey = &pKeys[i+1];
                fNextTime = pNextKey->GetTime() * TICKSPERSECOND;
            }
            else
            {
                pNextKey = NULL;
                fNextTime = fCurTime;
            }

            fTimeDelOut = fNextTime - fCurTime;

            fIn = pCurKey->GetInTan();
            pCurKey->SetInTan(-fIn*fTimeDelIn);
            fOut = pCurKey->GetOutTan();
            pCurKey->SetOutTan(fOut*fTimeDelOut);

            fTimeDelIn = fTimeDelOut;
            fLastTime = fCurTime;
            fCurTime = fNextTime;
            pCurKey = pNextKey;
        }
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;;
    pkInterp->SetKeys(pKeys, iNumKeys, NiFloatKey::BEZKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertLinFloat(IKeyControl* pIKeys)
{
    CHECK_MEMORY();
    int iNumKeys = pIKeys->GetNumKeys();

    NiLinFloatKey* pKeys = NiNew NiLinFloatKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    for (int i = 0; i < iNumKeys; i++)
    {
        ILinFloatKey lin;
        pIKeys->GetKey(i, &lin);

        pKeys[i].SetTime(lin.time * SECONDSPERTICK);
        pKeys[i].SetValue(lin.val);
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;;
    pkInterp->SetKeys(pKeys, iNumKeys, NiFloatKey::LINKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
// look in impexp/asciiexp/animout.cpp for a slightly better way to do this
NiInterpolator* NiMAXAnimationConverter::ConvertDumbFloat(Control* pFc)
{
    CHECK_MEMORY();

    TimeValue animStart;
    TimeValue animEnd;
    unsigned int uiNumFrames;
    CalculateControlTimeRange(pFc, animStart, animEnd, uiNumFrames);

    unsigned int uiMaxNumKeys = UINT_MAX / sizeof(NiLinFloatKey);
    if (uiNumFrames > uiMaxNumKeys)
    {
        // issue error message
        NiString strWarning = "A float controller contains animation data too "
            "large to fit into memory. Adjust the time range of animations "
            "and re-export.";
        NILOGWARNING((const char*)strWarning);
        return NULL;
    }

    NiLinFloatKey* pKeys = NiNew NiLinFloatKey[uiNumFrames];
    if (pKeys == NULL)
        return NULL;

    unsigned int i;
    TimeValue t;
    float fVal;
    Interval I;

    for (i = 0, t = animStart; 
        i < uiNumFrames - 1; 
        i++, t += GetTicksPerFrame())
    {
        pFc->GetValue(t, (void*)&fVal, I, CTRL_ABSOLUTE);

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetValue(fVal);
    }
    // the last frame of the sample should be exact time, not rounded to
    // the nearest frame
    pFc->GetValue(animEnd, (void*)&fVal, I, CTRL_ABSOLUTE);
    pKeys[uiNumFrames - 1].SetTime(animEnd * SECONDSPERTICK);
    pKeys[uiNumFrames - 1].SetValue(fVal);

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;;
    pkInterp->SetKeys(pKeys, uiNumFrames, NiFloatKey::LINKEY);
    return pkInterp;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::UnknownFloatAnim(Control* pFC)
{
    return ConvertDumbFloat(pFC);
}

//---------------------------------------------------------------------------
// "pNode" & "pName" should go away when OLD_PRINT is removed
NiInterpolator* NiMAXAnimationConverter::ConvertFloatAnim(Control* pFc,
    bool bAllowSingleKeyedAnimation)
{
    CHECK_MEMORY();
    NiInterpolator* pkInterp = NULL;
    Class_ID id = pFc->ClassID();

    if (pFc)
    {
        IKeyControl* pIKeys;
        pIKeys = GetKeyControlInterface(pFc);
        if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() == 0)
            {
                if (bAllowSingleKeyedAnimation)
                {
                    Interval kInterval;
                    float fValue;
                    pFc->GetValue(0, &fValue, kInterval);
                    NiFloatInterpolator* pkInterp = 
                        NiNew NiFloatInterpolator();
                    NiLinFloatKey* pkKeys = NiNew NiLinFloatKey[1];
                    pkKeys->SetTime(0.0f);
                    pkKeys->SetValue(fValue);
                    pkInterp->SetKeys(pkKeys, 1, NiAnimationKey::LINKEY);
                    return pkInterp;
                }
                else
                {
                    // We will not generate a single keyed animation
                    // here. That would just clutter up the scene graph
                    // (esp. wrt viscontrollers.)
                    return NULL;
                }
            }
            else if (id == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0))
                pkInterp = ConvertTCBFloat(pIKeys);
            else if (id == Class_ID(LININTERP_FLOAT_CLASS_ID, 0))
                pkInterp = ConvertLinFloat(pIKeys);
            else if (id == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0))
                pkInterp = ConvertBezFloat(pIKeys);
            else
                pkInterp = UnknownFloatAnim(pFc);
        }
        else
            pkInterp = UnknownFloatAnim(pFc);
    }
    else
        pkInterp = UnknownFloatAnim(pFc);
    
    return pkInterp;
}

//---------------------------------------------------------------------------
//                  POSITION ANIMATION
//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertTCBPos(Control*, 
    IKeyControl* pIKeys, ScaleInfo* pParentScale)
{
    CHECK_MEMORY();
    Matrix3 tm;

    int iNumKeys = pIKeys->GetNumKeys();
    NiTCBPosKey* pKeys = NiNew NiTCBPosKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;
    
    bool bUseMat= false;
    if (!pParentScale->bIsIdent)
    {
        Matrix3 srtm, stm, srtminv;
        Quat qinv;
    
        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);

        tm = srtm * stm * srtminv;
        bUseMat = true;
    }

    for (int i = 0; i < iNumKeys; i++)
    {
        ITCBPoint3Key tcb;
        NiPoint3 pos;

        pIKeys->GetKey(i, &tcb);

        if (bUseMat)
        {
            Point3 p;

            p = tcb.val * tm;
            pos.x = p.x;
            pos.y = p.y;
            pos.z = p.z;
        }
        else
        {
            pos.x = tcb.val.x;
            pos.y = tcb.val.y;
            pos.z = tcb.val.z;
        }

        pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
        pKeys[i].SetTension(tcb.tens);
        pKeys[i].SetContinuity(tcb.cont);
        pKeys[i].SetBias(tcb.bias);
    }

    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiPosKey::TCBKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertBezPos(Control*, 
    IKeyControl* pIKeys, ScaleInfo* pParentScale)
{ 
    CHECK_MEMORY();
    Matrix3 tm;

    int iNumKeys = pIKeys->GetNumKeys();
    NiBezPosKey* pKeys = NiNew NiBezPosKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    bool bUseMat = false;
    if (!pParentScale->bIsIdent)
    {
        Matrix3 srtm, stm, srtminv;
        Quat qinv;

        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;
        bUseMat = true;
    }

    int i = 0;
    for (; i < iNumKeys; i++)
    {
        IBezPoint3Key bez;
        NiPoint3 pos, in, out;

        pIKeys->GetKey(i, &bez);

        if (bUseMat)
        {
            Point3 p;
            
            p = bez.val * tm;
            pos.x = p.x;
            pos.y = p.y;
            pos.z = p.z;

            p = bez.intan * tm;
            in.x = p.x;
            in.y = p.y;
            in.z = p.z;
            
            p = bez.outtan * tm;
            out.x = p.x;
            out.y = p.y;
            out.z = p.z;
        }
        else
        {
            pos.x = bez.val.x;
            pos.y = bez.val.y;
            pos.z = bez.val.z;

            in.x = bez.intan.x;
            in.y = bez.intan.y;
            in.z = bez.intan.z;

            out.x = bez.outtan.x;
            out.y = bez.outtan.y;
            out.z = bez.outtan.z;
        }

        pKeys[i].SetTime(bez.time * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
        pKeys[i].SetInTan(in);
        pKeys[i].SetOutTan(out);
    }

    // We need to patch up the tangent vectors to match our interpolation
    // MAX's tangents are specified in ticks between samples and both 
    // vectors point into the segment. We convert this to 0..1 tangents
    // and the out tangent pointing away from the segment.
    if (iNumKeys)
    {
        float fLastTime, fCurTime, fNextTime, fTimeDelIn, fTimeDelOut;
        NiPoint3 inTan, outTan;

        fLastTime = fCurTime = pKeys[0].GetTime() * TICKSPERSECOND;
        NiBezPosKey* pCurKey = &pKeys[0];
        NiBezPosKey* pNextKey;
        fTimeDelIn = fCurTime - fLastTime;

        for (i = 0; i < iNumKeys; i++)
        {
            if (i+1 < iNumKeys)
            {
                pNextKey = &pKeys[i+1];
                fNextTime = pNextKey->GetTime() * TICKSPERSECOND;
            }
            else
            {
                pNextKey = NULL;
                fNextTime = fCurTime;
            }

            fTimeDelOut = fNextTime - fCurTime;

            inTan = pCurKey->GetInTan();
            pCurKey->SetInTan(-fTimeDelIn*inTan);
            outTan = pCurKey->GetOutTan();
            pCurKey->SetOutTan(fTimeDelOut*outTan);

            fTimeDelIn = fTimeDelOut;
            fLastTime = fCurTime;
            fCurTime = fNextTime;
            pCurKey = pNextKey;
        }
    }

    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiPosKey::BEZKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertLinPos(Control*,
    IKeyControl* pIKeys, ScaleInfo* pParentScale)
{
    CHECK_MEMORY();
    Matrix3 tm;

    int iNumKeys = pIKeys->GetNumKeys();
    NiLinPosKey* pKeys = NiNew NiLinPosKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    bool bUseMat = false;
    if (!pParentScale->bIsIdent)
    {
        Matrix3 srtm, stm, srtminv;
        Quat qinv;

        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;
        bUseMat = true;
    }

    for (int i = 0; i < iNumKeys; i++)
    {
        ILinPoint3Key lin;
        NiPoint3 pos;
        
        pIKeys->GetKey(i, &lin);
        
        if (bUseMat)
        {
            Point3 p;
    
            p = lin.val * tm;
    
            pos.x = p.x;
            pos.y = p.y;
            pos.z = p.z;
        }
        else
        {
            pos.x = lin.val.x;
            pos.y = lin.val.y;
            pos.z = lin.val.z;
        }

        pKeys[i].SetTime(lin.time * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
    }

    
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiPosKey::LINKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertDumbPos(Control *pPC,
    INode* pMaxNode, ScaleInfo* pParentScale)
{
    CHECK_MEMORY();
    TimeValue animStart;
    TimeValue animEnd;
    unsigned int uiNumFrames;
    CalculateControlTimeRange(pPC, animStart, animEnd, uiNumFrames);

    if (uiNumFrames == 0)
        return NULL;

    NiLinPosKey *pKeys = NULL;

    unsigned int uiMaxNumKeys = UINT_MAX / sizeof(NiLinPosKey);
    if (uiNumFrames > uiMaxNumKeys)
    {
        // issue error message
        NiString strWarning = "The object \"";
        strWarning += (const char*)pMaxNode->GetName();
        strWarning += "\" Contains animation data too large to fit "
            "into memory. Adjust the time range of animations and re-export.";
        NILOGWARNING((const char*)strWarning);
        return NULL;
    }

    try
    {
        pKeys = NiNew NiLinPosKey[uiNumFrames];
    }
    catch(...)
    {
        return NULL;
    }

    if (pKeys == NULL)
        return NULL;

    bool bKeysTheSameValue = true;
    unsigned int i;
    TimeValue t;
    NiPoint3 kLastPos;
    Matrix3 tm;
    AffineParts parts;
    NiPoint3 pos;

    for (i = 0, t = animStart; 
         i < uiNumFrames - 1; 
         i++, t += GetTicksPerFrame())
    {
        tm = NiMAXHierConverter::GetLocalTM(pMaxNode, t, pParentScale);
        decomp_affine(tm, &parts);

        pos.x = parts.t.x;
        pos.y = parts.t.y;
        pos.z = parts.t.z;

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
        if (i != 0 && kLastPos != pos)
            bKeysTheSameValue = false;
        kLastPos = pos;
    }
    // the last frame of the sample should be exact time, not rounded to
    // the nearest frame
    tm = NiMAXHierConverter::GetLocalTM(pMaxNode, animEnd, pParentScale);
    decomp_affine(tm, &parts);

    pos.x = parts.t.x;
    pos.y = parts.t.y;
    pos.z = parts.t.z;

    pKeys[uiNumFrames - 1].SetTime(animEnd * SECONDSPERTICK);
    pKeys[uiNumFrames - 1].SetPos(pos);
    if (kLastPos != pos)
        bKeysTheSameValue = false;

    if (!bKeysTheSameValue)
    {
        NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
        pkInterp->SetKeys(pKeys, uiNumFrames, NiPosKey::LINKEY);
        return pkInterp;
    }
    else
    {
        if (pKeys)
            NiDelete[] pKeys;
        return NULL;
    }
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::UnknownPosAnim(Control* pPC,
    INode* pMaxNode, ScaleInfo* pParentScale)
{
    return ConvertDumbPos(pPC, pMaxNode, pParentScale);
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertCompositePosAnim(
    Control*, Control* pXc, Control* pYc, Control* pZc,
    INode*, ScaleInfo* pParentScale)
{
    CHECK_MEMORY();
    IKeyControl* pIXKeys = NULL;
    IKeyControl* pIYKeys = NULL;
    IKeyControl* pIZKeys = NULL;
    NiFloatKey* pkXFloatKeys = NULL;
    NiFloatKey* pkYFloatKeys = NULL;
    NiFloatKey* pkZFloatKeys = NULL;
    NiAnimationKey::KeyType eXFloatKeyType = NiAnimationKey::NOINTERP;
    NiAnimationKey::KeyType eYFloatKeyType = NiAnimationKey::NOINTERP;
    NiAnimationKey::KeyType eZFloatKeyType = NiAnimationKey::NOINTERP;
    unsigned int uiXNumKeys = 0;
    unsigned int uiYNumKeys = 0;
    unsigned int uiZNumKeys = 0;
    unsigned char ucXSize = 0;
    unsigned char ucYSize = 0;
    unsigned char ucZSize = 0;

    if (pXc)
    {
            pIXKeys = GetKeyControlInterface(pXc);
            if (pIXKeys && pIXKeys->GetNumKeys() > 1)
            {
                NiInterpolatorPtr spInterp = ConvertFloatAnim(pXc);
                NiFloatInterpolator* pkFloatInterp = NiDynamicCast(
                    NiFloatInterpolator, spInterp);
                if (pkFloatInterp)
                {
                    pkXFloatKeys = (NiFloatKey*) pkFloatInterp->GetAnim(
                        uiXNumKeys, eXFloatKeyType, ucXSize, 0);
                    // transfer ownership of the keys
                    pkFloatInterp->SetKeys(NULL, 0, 
                        NiAnimationKey::NOINTERP);
                }
            }
    }

    if (pYc)
    {
            pIYKeys = GetKeyControlInterface(pYc);
            if (pIYKeys && pIYKeys->GetNumKeys() > 1)
            {
                NiInterpolatorPtr spInterp = ConvertFloatAnim(pYc);
                NiFloatInterpolator* pkFloatInterp = NiDynamicCast(
                    NiFloatInterpolator, spInterp);
                if (pkFloatInterp)
                {
                    pkYFloatKeys = (NiFloatKey*) pkFloatInterp->GetAnim(
                        uiYNumKeys, eYFloatKeyType, ucYSize, 0);
                    // transfer ownership of the keys
                    pkFloatInterp->SetKeys(NULL, 0, 
                        NiAnimationKey::NOINTERP);
                }
            }
    }

    if (pZc)
    {
            pIZKeys = GetKeyControlInterface(pZc);
            if (pIZKeys && pIZKeys->GetNumKeys() > 1)
            {
                NiInterpolatorPtr spInterp = ConvertFloatAnim(pZc);
                NiFloatInterpolator* pkFloatInterp = NiDynamicCast(
                    NiFloatInterpolator, spInterp);
                if (pkFloatInterp)
                {
                    pkZFloatKeys = (NiFloatKey*) pkFloatInterp->GetAnim(
                        uiZNumKeys, eZFloatKeyType, ucZSize, 0);
                    // transfer ownership of the keys
                    pkFloatInterp->SetKeys(NULL, 0, 
                        NiAnimationKey::NOINTERP);
                }
            }
    }

    if (uiXNumKeys == 0 && uiYNumKeys == 0 && uiZNumKeys == 0)
        return NULL;

    NiPosKey* pkFinalKeys = NULL;
    unsigned int uiNumFinalKeys = 0;
    NiPosKey::KeyType eFinalKeyType;

    NiOptimizeAnimation::MergeXYZFloatKeys(pkXFloatKeys, pkYFloatKeys, 
        pkZFloatKeys, eXFloatKeyType, eYFloatKeyType, eZFloatKeyType, 
        uiXNumKeys, uiYNumKeys, uiZNumKeys, pkFinalKeys, eFinalKeyType, 
        uiNumFinalKeys);
    
    bool bUseMat = false;
    Matrix3 tm;
    if (!pParentScale->bIsIdent)
    {
        Matrix3 srtm, stm, srtminv;
        Quat qinv;

        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;
        bUseMat = true;
    }

    unsigned char ucFinalKeySize = NiPosKey::GetKeySize(
        eFinalKeyType);
    
    if (bUseMat && pkFinalKeys != NULL)
    {
        for (unsigned int ui = 0; ui < uiNumFinalKeys; ui++)
        {
            NiPosKey* pkKey = pkFinalKeys->GetKeyAt(ui, ucFinalKeySize);
            NIASSERT(pkKey);
            NiPoint3 kSrcPos = pkKey->GetPos();
            Point3 kPosVal(kSrcPos.x, kSrcPos.y, kSrcPos.z);
            Point3 p;

            p = kPosVal * tm;
            kSrcPos.x = p.x;
            kSrcPos.y = p.y;
            kSrcPos.z = p.z;
            pkKey->SetPos(kSrcPos);

            if (eFinalKeyType == NiPosKey::BEZKEY)
            {
                NiBezPosKey* pkBezKey = (NiBezPosKey*) pkKey;
                NiPoint3 kSrcInTan = pkBezKey->GetInTan();
                NiPoint3 kSrcOutTan = pkBezKey->GetOutTan();

                Point3 kInTan(kSrcInTan.x, kSrcInTan.y, kSrcInTan.z);
                Point3 kOutTan(kSrcOutTan.x, kSrcOutTan.y, kSrcOutTan.z);

                p = kInTan * tm;
                kSrcInTan.x = p.x;
                kSrcInTan.y = p.y;
                kSrcInTan.z = p.z;

                p = kOutTan * tm;
                kSrcOutTan.x = p.x;
                kSrcOutTan.y = p.y;
                kSrcOutTan.z = p.z;

                pkBezKey->SetInTan(kSrcInTan);
                pkBezKey->SetOutTan(kSrcOutTan);

            }
        }
    }

    if (pkXFloatKeys)
    {
        NiFloatKey::DeleteFunction pfnXDelete = 
            NiFloatKey::GetDeleteFunction (eXFloatKeyType);
        pfnXDelete(pkXFloatKeys);
    }
    if (pkYFloatKeys)
    { 
        NiFloatKey::DeleteFunction pfnYDelete = 
            NiFloatKey::GetDeleteFunction (eYFloatKeyType);
        pfnYDelete(pkYFloatKeys);
    }
    if (pkZFloatKeys)
    {
        NiFloatKey::DeleteFunction pfnZDelete = 
            NiFloatKey::GetDeleteFunction (eZFloatKeyType);
        pfnZDelete(pkZFloatKeys);
    }

    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pkFinalKeys, uiNumFinalKeys, eFinalKeyType);
    return pkInterp;
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiMAXAnimationConverter::ConvertPositionAnim(
    Control* pPc, NiTimeController::CycleType& eCycleType, 
    INode* pMaxNode, ScaleInfo* pParentScale, bool &bSampled)
{
    NiInterpolatorPtr spInterp;

    bSampled = false;
    if (pMaxNode->UserPropExists("NiSamplePosKeys"))
    {
        bSampled = true;
    }

    if (pPc && !bSampled)
    {
        Class_ID id;

        id = pPc->ClassID(); 

        Control* pXc = pPc->GetXController();
        Control* pYc = pPc->GetYController();
        Control* pZc = pPc->GetZController();
        
        IListControl* pIListCtrl = GetIListControlInterface(pPc);
        IKeyControl* pIKeys = GetKeyControlInterface(pPc);

        if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() == 0)
            {
                return NULL;
            }
            else if (id == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0))
            {
                spInterp = ConvertTCBPos(pPc, pIKeys,
                                        pParentScale);
            }
            else if (id == Class_ID(LININTERP_POSITION_CLASS_ID, 0))
            {
                spInterp = ConvertLinPos(pPc, pIKeys,
                                        pParentScale);
            }
            else if (id == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
            {
                spInterp = ConvertBezPos(pPc, pIKeys,
                                        pParentScale);
            }
            else
            {
                spInterp = UnknownPosAnim(pPc, pMaxNode, 
                                         pParentScale);
                bSampled = true;
            }

        }
        else if (pXc || pYc || pZc)
        {
            bool bSampleKeys = false;
            IKeyControl* pIXKeys = NULL;
            IKeyControl* pIYKeys = NULL;
            IKeyControl* pIZKeys = NULL;

            // check if there are animation keys in the controller
            if (pXc)
            {
                 SetORT(pXc, eCycleType);
                 pIXKeys = GetKeyControlInterface(pXc);
                 if (pIXKeys && pIXKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIXKeys)
                     bSampleKeys = true;
            }

            if (pYc)
            {
                 SetORT(pYc, eCycleType);
                 pIYKeys = GetKeyControlInterface(pYc);
                 if (pIYKeys && pIYKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIYKeys)
                     bSampleKeys = true;
            }

            if (pZc)
            {
                 SetORT(pZc, eCycleType);
                 pIZKeys = GetKeyControlInterface(pZc);
                 if (pIZKeys && pIZKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIZKeys)
                     bSampleKeys = true;
            }

            // if this controller is animated, sample
            if (bSampleKeys)
            {
                spInterp = UnknownPosAnim(pPc, pMaxNode, 
                                         pParentScale);
                bSampled = true;
            }
            // if the controller is valid, but non-animated 
            else
            {
                return NULL;
            }
        }
        // Removed this code for the time being to keep the code from
        // creating the much larger BezPosKeys over LinPosKeys
        /*else if (pXc || pYc || pZc)
        {
            spInterp = ConvertCompositePosAnim(pPc, pXc, pYc, pZc, pMaxNode, 
                                        pParentScale);
        }*/
        // check if exactly one of the ListController's channels are animated
        else if (pIListCtrl && CanConvertPosListControl(pIListCtrl))
        {
            // if so, try to average the ListController's other channel's
            // poses into the animated track
            spInterp = ConvertPositionListAnim(pIListCtrl, eCycleType, 
                pMaxNode, pParentScale);

            if (!spInterp)
            {
                spInterp = UnknownPosAnim(pPc, pMaxNode, 
                                 pParentScale);
                bSampled = true;
            }
        }
        else
        {
            spInterp = UnknownPosAnim(pPc, pMaxNode, 
                                     pParentScale);
            bSampled = true;
        }
    }
    else
    {
        spInterp = UnknownPosAnim(pPc, pMaxNode, 
                                 pParentScale);
        bSampled = true;
    }

    if (!spInterp)
    {
        return NULL;
    }

    NiPoint3Interpolator* pkKeyInterp = NiDynamicCast(NiPoint3Interpolator, 
        spInterp);

    if (pkKeyInterp && pkKeyInterp->GetKeyCount(0))
    {
        pkKeyInterp->FillDerivedValues(0);
        SetORT(pPc, eCycleType);
    }
    
    return spInterp;
}

//---------------------------------------------------------------------------
//                  PATH ANIMATION
//---------------------------------------------------------------------------

void NiMAXAnimationConverter::XformVert(NiPoint3* pPos, PRSInfo* pPRS)
{
    Matrix3 rot;
    NiMatrix3 niRot;
    NiPoint3 temp;

    pPRS->q.MakeMatrix(rot);
    MaxToNI(rot, niRot);
    temp = niRot * (*pPos);
    temp *= pPRS->s;
    temp += pPRS->t;
    *pPos = temp;
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::XformVector(NiPoint3* pVec, PRSInfo* pPRS)
{
    Matrix3 rot;
    NiMatrix3 niRot;
    NiPoint3 temp;

    pPRS->q.MakeMatrix(rot);
    MaxToNI(rot, niRot);
    temp = niRot * (*pVec);
    *pVec = temp;
}

//---------------------------------------------------------------------------
int NiMAXAnimationConverter::GetXform(INode *pControlledMaxNode, 
    INode *pShapeMaxNode, PRSInfo *pResult, TimeValue animStart, 
    ScaleInfo* pParentScale)
{
    Matrix3 parentTM;
    Matrix3 scaleTM = pShapeMaxNode->GetNodeTM(animStart);
    if (pControlledMaxNode->GetParentNode())
    {
        scaleTM = scaleTM * 
        Inverse(pControlledMaxNode->GetParentNode()->GetNodeTM(animStart));
    }

    Matrix3 srtm, stm, srtminv;
    stm = ScaleMatrix(pParentScale->scale.s);
    pParentScale->scale.q.MakeMatrix(srtm);
    Quat qinv;        
    qinv = Inverse(pParentScale->scale.q);
    qinv.MakeMatrix(srtminv);

    scaleTM = scaleTM * srtm * stm * srtminv;
    AffineParts parts;
    decomp_affine(scaleTM, &parts);
    
    Quat q;
    Point3 p, s;
    q = parts.q;
    p = parts.t;
    s = parts.f < 0 ? -parts.k : parts.k;

    pResult->q = q;
    pResult->t.x = p.x;
    pResult->t.y = p.y;
    pResult->t.z = p.z;
    pResult->s = 1.0f;

    if (NiOptimize::CloseTo(s.x, s.y) && NiOptimize::CloseTo(s.y, s.z) &&
        NiOptimize::CloseTo(s.x, s.z))
    {
        if (s.x < 0.0)
            s.x = -s.x;
        pResult->s = s.x;
    }

    return(W3D_STAT_OK);
}
        
//---------------------------------------------------------------------------
bool NiMAXAnimationConverter::InvalidTans(NiPoint3& in, NiPoint3& out)
{
    // the tangents are invalid if either "in" or
    // "out" are the zero vector
    if (((in.x < NiOptimize::ms_fEpsilon && 
        in.x > -NiOptimize::ms_fEpsilon) &&
        (in.y < NiOptimize::ms_fEpsilon && 
        in.y > -NiOptimize::ms_fEpsilon) &&
        (in.z < NiOptimize::ms_fEpsilon && 
        in.z > -NiOptimize::ms_fEpsilon)) ||
        ((out.x < NiOptimize::ms_fEpsilon && 
        out.x > -NiOptimize::ms_fEpsilon) &&
        (out.y < NiOptimize::ms_fEpsilon && 
        out.y > -NiOptimize::ms_fEpsilon) &&
        (out.z < NiOptimize::ms_fEpsilon && 
        out.z > -NiOptimize::ms_fEpsilon)))
    {
        return(TRUE);
    }

    return(FALSE);
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::FixTans(Spline3D* pSpline, int iKnot, 
    NiPoint3* pIn, NiPoint3* pOut)
{
    NiPoint3 v1, v2;

    int iNumKnots = pSpline->KnotCount();
    SplineKnot curKnot = pSpline->GetKnot(iKnot);

    if ((iKnot == 0 || iKnot == iNumKnots-1) && !pSpline->Closed())
    {
        // handle the open edges
        if (iKnot == 0)
        {
            *pIn = NiPoint3::ZERO;
            SplineKnot nextKnot = pSpline->GetKnot((iKnot + 1) % iNumKnots);

            v1.x = nextKnot.Knot().x - curKnot.Knot().x;
            v1.y = nextKnot.Knot().y - curKnot.Knot().y;
            v1.z = nextKnot.Knot().z - curKnot.Knot().z;

            v1.Unitize();
            *pOut = v1;
        }
        else
        {
            *pOut = NiPoint3::ZERO;
            SplineKnot lastKnot = pSpline->GetKnot((iKnot + iNumKnots - 1) % 
                                                    iNumKnots);

            v2.x = lastKnot.Knot().x - curKnot.Knot().x;
            v2.y = lastKnot.Knot().y - curKnot.Knot().y;
            v2.z = lastKnot.Knot().z - curKnot.Knot().z;

            v2.Unitize();
            *pIn = -v2;
        }

        return;
    }

    SplineKnot nextKnot = pSpline->GetKnot((iKnot + 1) % iNumKnots);
    SplineKnot lastKnot = pSpline->GetKnot((iKnot + iNumKnots - 1) % 
                                            iNumKnots);

    v1.x = nextKnot.Knot().x - curKnot.Knot().x;
    v1.y = nextKnot.Knot().y - curKnot.Knot().y;
    v1.z = nextKnot.Knot().z - curKnot.Knot().z;

    v2.x = lastKnot.Knot().x - curKnot.Knot().x;
    v2.y = lastKnot.Knot().y - curKnot.Knot().y;
    v2.z = lastKnot.Knot().z - curKnot.Knot().z;

    v1.Unitize();
    v2.Unitize();

    *pIn = -v2;
    *pOut = v1;
}

//---------------------------------------------------------------------------
int NiMAXAnimationConverter::ConvertSpline3D(Spline3D* pSpline, 
    NiPosKey** ppPosKeys, unsigned int* pNumPosKeys,
    NiAnimationKey::KeyType* pPosKeyType, unsigned char& ucPosSize,
    PRSInfo *pPRS)
{
    int i, iTot;

    int iNumKnots = pSpline->KnotCount();
    if (pSpline->Closed())
        iTot = iNumKnots+1;
    else
        iTot = iNumKnots;

    *pNumPosKeys = iTot;
    *pPosKeyType = NiPosKey::BEZKEY;
    *ppPosKeys = (NiPosKey*) NiNew NiBezPosKey[iTot];
    ucPosSize = sizeof(NiBezPosKey);

    if (*ppPosKeys == NULL)
        return(W3D_STAT_NO_MEMORY);

    for (i = 0; i < iNumKnots; i++)
    {
        SplineKnot sKnot = pSpline->GetKnot(i);
        NiPoint3 pos, in, out;

        pos.x = sKnot.Knot().x;
        pos.y = sKnot.Knot().y;
        pos.z = sKnot.Knot().z;

        in.x = sKnot.Knot().x - sKnot.InVec().x;
        in.y = sKnot.Knot().y - sKnot.InVec().y;
        in.z = sKnot.Knot().z - sKnot.InVec().z;

        out.x = sKnot.OutVec().x - sKnot.Knot().x;
        out.y = sKnot.OutVec().y - sKnot.Knot().y;
        out.z = sKnot.OutVec().z - sKnot.Knot().z;

        if (InvalidTans(in, out))
            FixTans(pSpline, i, &in, &out);

        XformVert(&pos, pPRS);
        XformVector(&in, pPRS);
        XformVector(&out, pPRS);

        NiBezPosKey* pkBezPosKey = (NiBezPosKey*) (*ppPosKeys)->GetKeyAt(i,
            sizeof(NiBezPosKey));
        pkBezPosKey->SetTime(i / ((float) iTot-1.0f));
        pkBezPosKey->SetPos(pos);
        pkBezPosKey->SetInTan(in);
        pkBezPosKey->SetOutTan(out);
    }
    
    if (pSpline->Closed())
    {
        SplineKnot sKnot = pSpline->GetKnot(0);
        NiPoint3 pos, in, out;

        pos.x = sKnot.Knot().x;
        pos.y = sKnot.Knot().y;
        pos.z = sKnot.Knot().z;

        in.x = sKnot.Knot().x - sKnot.InVec().x;
        in.y = sKnot.Knot().y - sKnot.InVec().y;
        in.z = sKnot.Knot().z - sKnot.InVec().z;

        out.x = sKnot.OutVec().x - sKnot.Knot().x;
        out.y = sKnot.OutVec().y - sKnot.Knot().y;
        out.z = sKnot.OutVec().z - sKnot.Knot().z;

        if (InvalidTans(in, out))
            FixTans(pSpline, 0, &in, &out);

        XformVert(&pos, pPRS);
        XformVector(&in, pPRS);
        XformVector(&out, pPRS);

        NiBezPosKey* pkBezPosKey = (NiBezPosKey*)
            (*ppPosKeys)->GetKeyAt(iTot-1, sizeof(NiBezPosKey));
        pkBezPosKey->SetTime(1.0f);
        pkBezPosKey->SetPos(pos);
        pkBezPosKey->SetInTan(in);
        pkBezPosKey->SetOutTan(out);
    }

    // We need to convert from bezier curves to hermite
    if (iTot)
    {
        NiBezPosKey *pCurKey;
        NiPoint3 inTan, outTan;

        for (i = 0; i < iTot; i++)
        {
            pCurKey = (NiBezPosKey*) (*ppPosKeys)->GetKeyAt(i,
                sizeof(NiBezPosKey));

            inTan = pCurKey->GetInTan();        
            pCurKey->SetInTan(3.0f * inTan);

            outTan = pCurKey->GetOutTan();
            pCurKey->SetOutTan(3.0f * outTan);
        }
    }

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXAnimationConverter::ConvertBezierShape(BezierShape *pBS, 
    NiPosKey** ppPosKeys, unsigned int* pNumPosKeys,
    NiAnimationKey::KeyType* pPosKeyType, unsigned char& ucPosSize,
    PRSInfo* pPRS)
{
    NIASSERT(pBS->splineCount >= 1);

    // ignore all but the first spline
    int iStatus = ConvertSpline3D(pBS->GetSpline(0),
                              ppPosKeys, pNumPosKeys, pPosKeyType, ucPosSize,
                              pPRS);
    return(iStatus);
}

//---------------------------------------------------------------------------
int NiMAXAnimationConverter::ConvertShape(ShapeObject* pShape, 
    NiPosKey** ppPosKeys, unsigned int* pNumPosKeys,
    NiAnimationKey::KeyType* pPosKeyType,  unsigned char& ucPosSize,
    PRSInfo* pPRS, TimeValue animStart)
{
    int iStatus;

    *pNumPosKeys = 0;
    *ppPosKeys = NULL;

    pShape->SuperClassID(); 

    Class_ID id = pShape->ClassID();

    if (pShape->CanMakeBezier())
    {
        BezierShape bs;

        pShape->MakeBezier(animStart, bs);

        iStatus = ConvertBezierShape(&bs, ppPosKeys, pNumPosKeys, 
                                     pPosKeyType, ucPosSize, pPRS);
        if (iStatus)
            return(iStatus);
    }
    else
    {
        // Subclasses of SHAPE_CLASS_ID
        switch (id.PartA())
        {
        case SPLINESHAPE_CLASS_ID:
            iStatus = ConvertBezierShape(&((SplineShape *) pShape)->shape, 
                              ppPosKeys, pNumPosKeys, pPosKeyType, ucPosSize,
                              pPRS);
            if (iStatus)
                return(iStatus);
            break;
        case SPLINE3D_CLASS_ID:
            iStatus = ConvertSpline3D((Spline3D *) pShape,
                                        ppPosKeys, 
                                        pNumPosKeys, 
                                        pPosKeyType,
                                        ucPosSize,
                                        pPRS);
            if (iStatus)
                return(iStatus);
            break;
        case NGON_CLASS_ID:
            {
                BezierShape bs;
                
                ((SimpleSpline *) pShape)->BuildShape(animStart, bs);
                
                iStatus = ConvertBezierShape(&bs, 
                                             ppPosKeys, 
                                             pNumPosKeys, 
                                             pPosKeyType,
                                             ucPosSize,
                                             pPRS);
                if (iStatus)
                    return(iStatus);
            }
            break;
        case LINEARSHAPE_CLASS_ID:
        case DONUT_CLASS_ID:
        case STAR_CLASS_ID:
        case RECTANGLE_CLASS_ID:
        case HELIX_CLASS_ID:
        case ELLIPSE_CLASS_ID:
        case CIRCLE_CLASS_ID:
        case TEXT_CLASS_ID:
        case ARC_CLASS_ID:
        default:
            {
                Class_ID tempId(SPLINESHAPE_CLASS_ID, 0);
                if (pShape && pShape->CanConvertToType(tempId))
                {
                    SplineShape * pkSpline = (SplineShape *) 
                        pShape->ConvertToType(0, tempId);
                    if (pkSpline)
                    {
                        iStatus = ConvertBezierShape(&((SplineShape *) 
                            pkSpline)->shape, ppPosKeys, pNumPosKeys,
                            pPosKeyType, ucPosSize, pPRS);

                        if (pkSpline && pkSpline != pShape)
                            pkSpline->DeleteThis();
                        if (iStatus)
                            return(iStatus);
                    }

                }
            }
            break;
        }       
    }
    
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXAnimationConverter::ConvertPath(INode *pMaxNode, Control* pControl,
    NiPathInterpolator* pPathInterpolator, 
    NiTimeController::CycleType& eCycleType, ScaleInfo* pParentScale)
{
    IPathPosition* pPath = (IPathPosition*) pControl;
    INode *pNode;
    int iStatus;
    Control *pCont;

    NiFloatKey* pPctKeys = NULL;
    NiFloatKey::KeyType ePctType = NiFloatKey::NOINTERP;
    unsigned int uiNumPctKeys = 0;
    unsigned char ucPctSize = 0;

    bool bFollow = pPath->GetFollow() ? true : false;
    bool bBank = pPath->GetBank() ? true : false;
    float fBankAmt = pPath->GetBankAmount();
    float fTrack = pPath->GetTracking();
    bool bAllowFlip = pPath->GetAllowFlip() ? true : false;
    bool bConstVel = pPath->GetConstVel() ? true : false;
    bool bFlip = pPath->GetFlip() ? true : false;
    int iAxis = pPath->GetAxis();
    bool bLoop = pPath->GetLoop() ? true : false;

    {
        IParamBlock2* pBlock;

        pBlock = (IParamBlock2 *) pPath->GetReference(PATHPOS_PBLOCK_REF);

        pCont = pBlock->GetController(path_percent, 0);

        NiInterpolatorPtr spPctInterp = ConvertFloatAnim(pCont, true);
        
        if (!spPctInterp)
            return W3D_STAT_FAILED;

        NiFloatInterpolator* pkInterp = NiDynamicCast(NiFloatInterpolator,
            spPctInterp);
        if (pkInterp && pkInterp->GetKeyCount(0) != 0)
        {
            pkInterp->FillDerivedValues(0);
            pPctKeys = (NiFloatKey*) pkInterp->GetAnim(uiNumPctKeys, 
                ePctType, ucPctSize, 0);
            // Transfer ownership of keys
            pkInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
        }
    }

    // MAX 4 supports multiple targets, but NI currently only supports one
    pNode = pPath->GetNode(0);

    if (pNode == NULL)
    {
        return(W3D_STAT_FAILED);
    }

    ObjectState kOState = pNode->EvalWorldState(m_animStart);
    Object* pObj = kOState.obj;

    NiPosKey* pkPosKeys = NULL;
    unsigned int uiNumPosKeys = 0;
    NiAnimationKey::KeyType ePosType = NiAnimationKey::NOINTERP;
    unsigned char ucPosSize = 0;

    SClass_ID sID = pObj->SuperClassID(); 
    if (sID == SHAPE_CLASS_ID)
    {
        PRSInfo prs;

        iStatus = GetXform(pMaxNode, pNode, &prs, m_animStart, pParentScale);
        if (iStatus)
            return(iStatus);

        iStatus = ConvertShape((ShapeObject *) pObj, 
            &pkPosKeys, &uiNumPosKeys, &ePosType, ucPosSize,
            &prs, m_animStart);
        if (iStatus)
            return(iStatus);
    }

    if (uiNumPosKeys == 0)
    {
        return(W3D_STAT_FAILED);
    }

    // precalculate the spline values
    NiPosKey::FillDerivedValsFunction pDeriv;
    
    pDeriv = NiPosKey::GetFillDerivedFunction(ePosType);
    NIASSERT(pDeriv);

    (*pDeriv)((NiAnimationKey*)pkPosKeys, uiNumPosKeys, ucPosSize);

    pPathInterpolator->ReplacePathKeys(pkPosKeys, uiNumPosKeys, ePosType);
    pPathInterpolator->ReplacePctKeys(pPctKeys, uiNumPctKeys, ePctType);

    pPathInterpolator->SetFollow(bFollow);
    pPathInterpolator->SetBank(bBank);
    if (fBankAmt < 0.0)
    {
        pPathInterpolator->SetBankDir(NiPathInterpolator::NEGATIVE);
        pPathInterpolator->SetMaxBankAngle(-fBankAmt * 0.01f);
    }
    else
    {
        pPathInterpolator->SetBankDir(NiPathInterpolator::POSITIVE);
        pPathInterpolator->SetMaxBankAngle(fBankAmt * 0.01f);
    }
    pPathInterpolator->SetSmoothing(fTrack);
    pPathInterpolator->SetAllowFlip(bAllowFlip);
    pPathInterpolator->SetConstVelocity(bConstVel);
    pPathInterpolator->SetFollowAxis((short)iAxis);
    pPathInterpolator->SetFlip(bFlip);

    //SetORT(pCont, pPathInterpolator);
    eCycleType = NiTimeController::CLAMP;
    if (bLoop)
        eCycleType = NiTimeController::LOOP;

    return(W3D_STAT_OK);
}


//---------------------------------------------------------------------------
//                  ROTATION ANIMATION
//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertTCBRot(IKeyControl* pIKeys)
{
    int iNumKeys = pIKeys->GetNumKeys();
    NiTCBRotKey* pKeys = NiNew NiTCBRotKey[iNumKeys];
    NIASSERT(pKeys);

    Quat accumQuat;
    accumQuat.Identity();
    for (int i = 0; i < iNumKeys; i++)
    {
        ITCBRotKey tcb;

        pIKeys->GetKey(i, &tcb);
        

        Quat curQuat = QFromAngAxis(tcb.val.angle, tcb.val.axis);
        accumQuat *= curQuat;
        accumQuat.Normalize();

        //
        // Use inverse quaternion and angle to match 
        // Gamebryo rotation direction
        //
        NiQuaternion q;

        q.SetW(accumQuat.w);
        q.SetX(-accumQuat.x);
        q.SetY(-accumQuat.y);
        q.SetZ(-accumQuat.z);

        pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        pKeys[i].SetQuaternion(q);
        pKeys[i].SetTension(tcb.tens);
        pKeys[i].SetContinuity(tcb.cont);
        pKeys[i].SetBias(tcb.bias);
    }

    NiQuaternionInterpolator* pkInterp = NiNew NiQuaternionInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiRotKey::TCBKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertBezRot(IKeyControl* pIKeys)
{
    int iNumKeys = pIKeys->GetNumKeys();
    NiBezRotKey* pKeys = NiNew NiBezRotKey[iNumKeys];
    NIASSERT(pKeys);

    for (int i = 0; i < iNumKeys; i++)
    {
        IBezQuatKey bez;
        NiQuaternion q;

        pIKeys->GetKey(i, &bez);

        //
        // Use inverse quaternion and angle to match 
        // Gamebryo rotation direction
        //
        q.SetW(bez.val.w);
        q.SetX(-bez.val.x);
        q.SetY(-bez.val.y);
        q.SetZ(-bez.val.z);

        pKeys[i].SetTime(bez.time * SECONDSPERTICK);
        pKeys[i].SetQuaternion(q);
    }

    NiQuaternionInterpolator* pkInterp = NiNew NiQuaternionInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiRotKey::BEZKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertLinRot(IKeyControl* pIKeys)
{
    int iNumKeys = pIKeys->GetNumKeys();
    NiLinRotKey* pKeys = NiNew NiLinRotKey[iNumKeys];
    NIASSERT(pKeys);

    for (int i = 0; i < iNumKeys; i++)
    {
        ILinRotKey lin;
        NiQuaternion q;

        pIKeys->GetKey(i, &lin);

        // Use inverse quaternion and angle to match 
        // Gamebryo rotation direction
        q.SetW(lin.val.w);
        q.SetX(-lin.val.x);
        q.SetY(-lin.val.y);
        q.SetZ(-lin.val.z);

        pKeys[i].SetTime(lin.time * SECONDSPERTICK);
        pKeys[i].SetQuaternion(q);
    }

    
    NiQuaternionInterpolator* pkInterp = NiNew NiQuaternionInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiRotKey::LINKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::MakeSingleKey(Control* pCont,
    TimeValue animStart, unsigned int* pNumKeys, NiFloatKey** ppKeys,
    NiAnimationKey::KeyType* pType)
{
    float fTmp;
    Interval I;

    pCont->GetValue(animStart, (void *) &fTmp, I, CTRL_ABSOLUTE);

    *pNumKeys = 1;
    *pType = NiFloatKey::LINKEY;
    (*ppKeys) = (NiFloatKey *) NiNew NiLinFloatKey[1];
    NIASSERT((*ppKeys));

    NiLinFloatKey* pkLinFloatKey = (NiLinFloatKey*) (*ppKeys)->GetKeyAt(0,
        sizeof(NiLinFloatKey));
    pkLinFloatKey->SetTime(animStart * SECONDSPERTICK);
    pkLinFloatKey->SetValue(fTmp);
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertEulerRot(Control* pRc,
    NiTimeController::CycleType& eCycleType)
{
    unsigned int uiNumKeys = 1;
    NiEulerRotKey* pRotKeys = NiNew NiEulerRotKey[1];

    Control* pX = pRc->GetXController();
    Control* pY = pRc->GetYController();
    Control* pZ = pRc->GetZController();

    unsigned int uiNumKeysX = 0;
    unsigned int uiNumKeysY = 0;
    unsigned int uiNumKeysZ = 0;
    NiFloatKey *pKeysX = NULL, *pKeysY = NULL, *pKeysZ = NULL;
    NiAnimationKey::KeyType 
        eTypeX = NiAnimationKey::NOINTERP, 
        eTypeY = NiAnimationKey::NOINTERP, 
        eTypeZ = NiAnimationKey::NOINTERP;
    unsigned char ucSizeX=0, ucSizeY=0, ucSizeZ=0;

    NiInterpolatorPtr spInterpX = ConvertFloatAnim(pX);
    NiInterpolatorPtr spInterpY = ConvertFloatAnim(pY);
    NiInterpolatorPtr spInterpZ = ConvertFloatAnim(pZ);

    TimeValue animStart = TIME_PosInfinity;

    if (spInterpX)
    {
        SetORT(pX, eCycleType);
        Interval kInterval = pX->GetTimeRange(TIMERANGE_ALL | 
            TIMERANGE_CHILDNODES | TIMERANGE_CHILDANIMS);
        if (kInterval.Start() < animStart)
            animStart = kInterval.Start();
    }
    if (spInterpY)
    {
        SetORT(pY, eCycleType);
        Interval kInterval = pY->GetTimeRange(TIMERANGE_ALL | 
            TIMERANGE_CHILDNODES | TIMERANGE_CHILDANIMS);
        if (kInterval.Start() < animStart)
            animStart = kInterval.Start();
    }
    if (spInterpZ)
    {
        SetORT(pZ, eCycleType);
        Interval kInterval = pZ->GetTimeRange(TIMERANGE_ALL | 
            TIMERANGE_CHILDNODES | TIMERANGE_CHILDANIMS);
        if (kInterval.Start() < animStart)
            animStart = kInterval.Start();
    }

    if (spInterpX && NiIsKindOf(NiFloatInterpolator, spInterpX))
    {
        NiFloatInterpolator* pkInterp = NiDynamicCast(NiFloatInterpolator,
            spInterpX);
        pKeysX = (NiFloatKey*)pkInterp->GetAnim(uiNumKeysX, eTypeX, ucSizeX,
            0);
        // Transfer ownership of the keys
        pkInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
    }
    if (spInterpY && NiIsKindOf(NiFloatInterpolator, spInterpY))
    {
        NiFloatInterpolator* pkInterp = NiDynamicCast(NiFloatInterpolator,
            spInterpY);
        pKeysY = (NiFloatKey*)pkInterp->GetAnim(uiNumKeysY, eTypeY, ucSizeY,
            0);
        // Transfer ownership of the keys
        pkInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);

    }
    if (spInterpZ  && NiIsKindOf(NiFloatInterpolator, spInterpZ))
    {
        NiFloatInterpolator* pkInterp = NiDynamicCast(NiFloatInterpolator,
            spInterpZ);
        pKeysZ = (NiFloatKey*)pkInterp->GetAnim(uiNumKeysZ, eTypeZ, ucSizeZ,
            0);
        // Transfer ownership of the keys
        pkInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
    }

    if (uiNumKeysX == 1 && uiNumKeysY == 1 && uiNumKeysZ == 1)
    {
        if (pKeysX->GetKeyAt(0, ucSizeX)->GetValue() <= 
                NiOptimize::ms_fEpsilon && 
            pKeysX->GetKeyAt(0, ucSizeX)->GetValue() >= 
                -NiOptimize::ms_fEpsilon && 
            pKeysY->GetKeyAt(0, ucSizeY)->GetValue() <= 
                NiOptimize::ms_fEpsilon && 
            pKeysY->GetKeyAt(0, ucSizeY)->GetValue() >= 
                -NiOptimize::ms_fEpsilon && 
            pKeysZ->GetKeyAt(0, ucSizeZ)->GetValue() <= 
                NiOptimize::ms_fEpsilon && 
            pKeysZ->GetKeyAt(0, ucSizeZ)->GetValue() >= 
                -NiOptimize::ms_fEpsilon)
        {
            NiDelete [] pKeysX;
            NiDelete [] pKeysY;
            NiDelete [] pKeysZ;
            NiDelete [] pRotKeys;
            return NULL;
        }
    }
    else if (uiNumKeysX == 0 && uiNumKeysY == 0 && uiNumKeysZ == 0)
    {
        NiDelete [] pRotKeys;
        return NULL;
    }

    // Since an euler controller will overwrite the entire transform
    // we need to grab the constant value for the non-animating axes.
    if (uiNumKeysX == 0)
        MakeSingleKey(pX, animStart, &uiNumKeysX, &pKeysX, &eTypeX);

    if (uiNumKeysY == 0)
        MakeSingleKey(pY, animStart, &uiNumKeysY, &pKeysY, &eTypeY);

    if (uiNumKeysZ == 0)
        MakeSingleKey(pZ, animStart, &uiNumKeysZ, &pKeysZ, &eTypeZ);

    NiEulerRotKey* pkEulerRotKey = pRotKeys;
    pkEulerRotKey->Initialize(pKeysX, uiNumKeysX, eTypeX,
                              pKeysY, uiNumKeysY, eTypeY,
                              pKeysZ, uiNumKeysZ, eTypeZ);

    NiQuaternionInterpolator* pkInterp = NiNew NiQuaternionInterpolator;
    pkInterp->SetKeys(pRotKeys, uiNumKeys, NiRotKey::EULERKEY);
    pkInterp->FillDerivedValues(0);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertDumbRot(Control* pControl,
    INode* pMaxNode, ScaleInfo* pParentScale)
{
    TimeValue animStart;
    TimeValue animEnd;
    unsigned int uiNumFrames;
    CalculateControlTimeRange(pControl, animStart, animEnd, uiNumFrames);

    if (uiNumFrames == 0)
        return NULL;

    unsigned int uiMaxNumKeys = UINT_MAX / sizeof(NiLinRotKey);
    if (uiNumFrames > uiMaxNumKeys)
    {
        // issue error message
        NiString strWarning = "The object \"";
        strWarning += (const char*)pMaxNode->GetName();
        strWarning += "\" Contains animation data too large to fit "
            "into memory. Adjust the time range of animations and re-export.";
        NILOGWARNING((const char*)strWarning);
        return NULL;
    }

    NiLinRotKey* pKeys = NiNew NiLinRotKey[uiNumFrames];
    NIASSERT(pKeys);
    NiQuaternion kLastRot;
    bool bAllSameValue = true;
    Matrix3 tm;
    AffineParts parts;
    NiQuaternion q;

    TimeValue t;
    unsigned int i;
    for (i = 0, t = animStart; 
         i < uiNumFrames - 1; 
         i++, t += GetTicksPerFrame())
    {
        tm = NiMAXHierConverter::GetLocalTM(pMaxNode, t, pParentScale);
        decomp_affine(tm, &parts);

        //
        // Use inverse quaternion and angle to match 
        // Gamebryo rotation direction
        //
        q.SetW(parts.q.w);
        q.SetX(-parts.q.x);
        q.SetY(-parts.q.y);
        q.SetZ(-parts.q.z);

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetQuaternion(q);

        if (i != 0 && q != kLastRot)
            bAllSameValue = false;

        kLastRot = q;
    }
    // the last frame of the sample should be exact time, not rounded to
    // the nearest frame
    tm = NiMAXHierConverter::GetLocalTM(pMaxNode, animEnd, pParentScale);
    decomp_affine(tm, &parts);

    q.SetW(parts.q.w);
    q.SetX(-parts.q.x);
    q.SetY(-parts.q.y);
    q.SetZ(-parts.q.z);

    pKeys[uiNumFrames - 1].SetTime(animEnd * SECONDSPERTICK);
    pKeys[uiNumFrames - 1].SetQuaternion(q);

    if (q != kLastRot)
        bAllSameValue = false;
    
    if (!bAllSameValue)
    {
        NiQuaternionInterpolator* pkInterp = NiNew NiQuaternionInterpolator;
        pkInterp->SetKeys(pKeys, uiNumFrames, NiRotKey::LINKEY);
        return pkInterp;
    }
    else
    {
        if (pKeys)
            NiDelete[] pKeys;
        return NULL;
    }
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::UnknownRotAnim(Control* pRC, 
    INode* pMaxNode, ScaleInfo* pParentScale)
{
    return ConvertDumbRot(pRC, pMaxNode, pParentScale);
}

//---------------------------------------------------------------------------
NiLookAtInterpolator* NiMAXAnimationConverter::ConvertLookAt(
    Control* pControl, NiTimeController::CycleType& eCycleType)
{
    NiLookAtInterpolator *pNewLookAt = NULL;
    NiInterpolator *pNewRoll = NULL;
    ILookatControl *pLAControl;
    INode *pTarget;
    Control *pRoll;
    BOOL bFlip;
    int iAxis;

    // first extract all the information from the lookat controller
    pLAControl = (ILookatControl *) pControl;

    pTarget = pLAControl->GetTarget();

    pRoll = pLAControl->GetRollController();

    bFlip = pLAControl->GetFlip();
    iAxis = pLAControl->GetAxis();

    // then make a roll controller if we need to. 
    if (pRoll)
    {
        pNewRoll = ConvertFloatAnim(pRoll);
        if (pNewRoll)
            pNewRoll->Collapse();
    }

    // third build a new look at controller
    pNewLookAt = NiNew NiLookAtInterpolator;
    NIASSERT(pNewLookAt);

    pNewLookAt->SetFlip(bFlip ? 1 : 0);
    switch (iAxis)
    {
    case 0:
        pNewLookAt->SetAxis(NiLookAtInterpolator::X);
        break;
    case 1:
        pNewLookAt->SetAxis(NiLookAtInterpolator::Y);
        break;
    case 2:
        pNewLookAt->SetAxis(NiLookAtInterpolator::Z);
        break;
    }

    if (pNewRoll)
        pNewLookAt->SetRollInterpolator(pNewRoll);
    
    SetORT(pLAControl->GetPositionController(), eCycleType);
    SetORT(pRoll, eCycleType);

    // need to put lookat control & target on a list to
    // resolve target pointer after all the node's are converted
    LookAtInfo *pLookAtInfo = NiNew LookAtInfo;
    if (pLookAtInfo)
    {
        pLookAtInfo->pLookAt = pTarget;
        pLookAtInfo->pControl = pNewLookAt;
        
        ms_lookAtList.AddHead(pLookAtInfo);
    }

    return pNewLookAt;
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiMAXAnimationConverter::ConvertRotationAnim(Control* pRc, 
    NiTimeController::CycleType& eCycleType, INode* pMaxNode, 
    ScaleInfo* pParentScale, int iMinNumKeys)
{
    IKeyControl *pIKeys;
    Class_ID id;
    
    NiRotKey *pkRotKeys;
    unsigned int uiNumRotKeys;
    NiAnimationKey::KeyType eRotType;
    BOOL bEuler = FALSE;

    NiInterpolatorPtr spInterp;
    NIASSERT(iMinNumKeys == 0 || iMinNumKeys == 1);

    pkRotKeys = NULL;
    uiNumRotKeys = 0;
    eRotType = NiAnimationKey::NOINTERP;

    bool bSampleRotKeys = false;
    if (pMaxNode->UserPropExists("NiSampleRotKeys"))
    {
        bSampleRotKeys = true;
    }

    if (pRc && !bSampleRotKeys)
    {
        id = pRc->ClassID();
    
        pIKeys = GetKeyControlInterface(pRc);
    
        if (id == Class_ID(EULER_CONTROL_CLASS_ID, 0) ||
            id == Class_ID(LOCAL_EULER_CONTROL_CLASS_ID,0))
        {
            bEuler = TRUE;
            spInterp = ConvertEulerRot(pRc, eCycleType);
            if (spInterp)
            {
                return(spInterp);
            }
        }
        else if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() <= iMinNumKeys)  
            {
                // if the # of keys is 0 or 1 then we can just rely on 
                // the rotation being stored in the scene graph.
                uiNumRotKeys = 0;
                spInterp = NULL;
            }
            else if (id == Class_ID(LININTERP_ROTATION_CLASS_ID, 0))
                spInterp = ConvertLinRot(pIKeys);
            else if (id == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0))
            {
                if (pMaxNode->UserPropExists("NiSampleTCBRotKeys"))
                {
                    spInterp = UnknownRotAnim(pRc, pMaxNode,
                                             pParentScale);
                }
                else
                {
                    spInterp = ConvertTCBRot(pIKeys);
                }
            }
            else if (id == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
                spInterp = ConvertBezRot(pIKeys);
            else
                spInterp = UnknownRotAnim(pRc, pMaxNode, pParentScale);
        }
        else
        {
            spInterp = UnknownRotAnim(pRc, pMaxNode, pParentScale);
        }
    }
    else
    {
        spInterp = UnknownRotAnim(pRc, pMaxNode, pParentScale);
    }

    if (!spInterp)
        return NULL;

    NiQuaternionInterpolator* pkQuatInterp = 
        NiDynamicCast(NiQuaternionInterpolator, spInterp);

    if (pkQuatInterp && pkQuatInterp->GetKeyCount(0) > 0)
    {
        pkQuatInterp->FillDerivedValues(0);
        SetORT(pRc, eCycleType);
        return spInterp;
    }
    else if (spInterp)
    {
        return spInterp;
    }

    return NULL;
    
}

//---------------------------------------------------------------------------
//                  VISIBILITY ANIMATION
//---------------------------------------------------------------------------
int NiMAXAnimationConverter::ConvertVisAnim(Control *pVc, 
    NiVisController *pVisController)
{
    NiFloatKey *pKeys;
    NiAnimationKey::KeyType eType;
    unsigned int uiNumKeys, i;
    unsigned char ucSize;

    NiInterpolatorPtr spInterp = ConvertFloatAnim(pVc);

    if (!spInterp)
        return W3D_STAT_OK;

    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(NiFloatInterpolator, 
        spInterp);

    if (!pkFloatInterp)
        return W3D_STAT_OK;

    pKeys = (NiFloatKey*)pkFloatInterp->GetAnim(uiNumKeys, eType, ucSize, 0);

    if (uiNumKeys)
    {
        NiStepBoolKey *pBoolKeys;

        pBoolKeys = (NiStepBoolKey *) NiNew NiStepBoolKey[uiNumKeys];
        if (pBoolKeys != NULL)
        {
            NiBoolInterpolator* pkInterp = NiNew NiBoolInterpolator;
            for (i = 0; i < uiNumKeys; i++)
            {
                pBoolKeys[i].SetTime(pKeys->GetKeyAt(i, ucSize)->GetTime());
                pBoolKeys[i].SetBool(pKeys->GetKeyAt(i, ucSize)->GetValue() >
                    0.0f ? TRUE : FALSE);
            }

#ifdef NIDEBUG
            NIASSERT(pVisController->GetInterpolator(0) == NULL);
#endif
            
            pkInterp->ReplaceKeys(pBoolKeys, uiNumKeys,
                NiAnimationKey::STEPKEY);
            
            pVisController->SetInterpolator(pkInterp);
            pVisController->ResetTimeExtrema();
        }

        SetORT(pVc, pVisController);
    }

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
//                  COLOR ANIMATION
//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertTCBPoint3(Control *, 
    IKeyControl *pIKeys)
{
    unsigned int i, uiNumKeys;
    NiTCBPosKey *pKeys;

    uiNumKeys = pIKeys->GetNumKeys();
    pKeys = NiNew NiTCBPosKey[uiNumKeys];
    if (pKeys == NULL)
        return NULL;
    
    for (i = 0; i < uiNumKeys; i++)
    {
        ITCBPoint3Key tcb;
        NiPoint3 pos;

        pIKeys->GetKey(i, &tcb);

        pos.x = tcb.val.x;
        pos.y = tcb.val.y;
        pos.z = tcb.val.z;

        pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
        pKeys[i].SetTension(tcb.tens);
        pKeys[i].SetContinuity(tcb.cont);
        pKeys[i].SetBias(tcb.bias);
    }

    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, uiNumKeys, NiPosKey::TCBKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertBezPoint3(Control *, 
    IKeyControl *pIKeys)
{
    NiBezPosKey *pKeys;
    unsigned int i, uiNumKeys;

    uiNumKeys = pIKeys->GetNumKeys();
    pKeys = NiNew NiBezPosKey[uiNumKeys];
    if (pKeys == NULL)
        return NULL;

    for (i = 0; i < uiNumKeys; i++)
    {
        IBezPoint3Key bez;
        NiPoint3 pos, in, out;

        pIKeys->GetKey(i, &bez);

        pos.x = bez.val.x;
        pos.y = bez.val.y;
        pos.z = bez.val.z;
        
        in.x = bez.intan.x;
        in.y = bez.intan.y;
        in.z = bez.intan.z;
        
        out.x = bez.outtan.x;
        out.y = bez.outtan.y;
        out.z = bez.outtan.z;
        
        pKeys[i].SetTime(bez.time * SECONDSPERTICK);
        pKeys[i].SetPos(pos);
        pKeys[i].SetInTan(in);
        pKeys[i].SetOutTan(out);
    }

    // We need to patch up the tangent vectors to match our interpolation
    // MAX's tangents are specified in ticks between samples and both 
    // vectors point into the segment. We convert this to 0..1 tangents
    // and the out tangent pointing away from the segment.
    if (uiNumKeys)
    {
        float fLastTime, fCurTime, fNextTime, fTimeDelIn, fTimeDelOut;
        NiBezPosKey *pCurKey, *pNextKey;
        NiPoint3 inTan, outTan;

        fLastTime = fCurTime = pKeys[0].GetTime() * TICKSPERSECOND;
        pCurKey = &pKeys[0];
        fTimeDelIn = fCurTime - fLastTime;

        for (i = 0; i < uiNumKeys; i++)
        {
            if (i+1 < uiNumKeys)
            {
                pNextKey = &pKeys[i+1];
                fNextTime = pNextKey->GetTime() * TICKSPERSECOND;
            }
            else
            {
                pNextKey = NULL;
                fNextTime = fCurTime;
            }

            fTimeDelOut = fNextTime - fCurTime;

            inTan = pCurKey->GetInTan();
            pCurKey->SetInTan(-fTimeDelIn*inTan);
            outTan = pCurKey->GetOutTan();
            pCurKey->SetOutTan(fTimeDelOut*outTan);

            fTimeDelIn = fTimeDelOut;
            fLastTime = fCurTime;
            fCurTime = fNextTime;
            pCurKey = pNextKey;
        }
    }

    
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, uiNumKeys, NiPosKey::BEZKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertDumbPoint3(Control *pCol)
{
    TimeValue animStart;
    TimeValue animEnd;
    unsigned int uiNumFrames;
    CalculateControlTimeRange(pCol, animStart, animEnd, uiNumFrames);

    if (uiNumFrames == 0)
        return NULL;

    unsigned int uiMaxNumKeys = UINT_MAX / sizeof(NiLinPosKey);
    if (uiNumFrames > uiMaxNumKeys)
    {
        // issue error message
        NiString strWarning = "A Point3 controller contains animation data "
            "too large to fit into memory. Adjust the time range of "
            "animations and re-export.";
        NILOGWARNING((const char*)strWarning);
        return NULL;
    }

    NiLinPosKey *pKeys;
    TimeValue t;
    unsigned int i;
    Point3 val;
    Interval range;

    pKeys = NiNew NiLinPosKey[uiNumFrames];
    if (pKeys == NULL)
        return NULL;

    for (i = 0, t = animStart; 
         i < uiNumFrames - 1; 
         i++, t += GetTicksPerFrame())
    {
        pCol->GetValue(t, &val, range);

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetPos(NiPoint3(val.x, val.y, val.z));
    }
    // the last frame of the sample should be exact time, not rounded to
    // the nearest frame
    pCol->GetValue(animEnd, &val, range);

    pKeys[uiNumFrames - 1].SetTime(animEnd * SECONDSPERTICK);
    pKeys[uiNumFrames - 1].SetPos(NiPoint3(val.x, val.y, val.z));

    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
    pkInterp->SetKeys(pKeys, uiNumFrames, NiPosKey::LINKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::UnknownColAnim(Control *pCol)
{
    return ConvertDumbPoint3(pCol);
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertPoint3Anim(Control *pCol)
{
    NiInterpolator* pkInterp;
    IKeyControl *pIKeys;

    if (pCol)
    {
        Class_ID id;

        id = pCol->ClassID(); 

        pIKeys = GetKeyControlInterface(pCol);
        if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() == 0)
                return(W3D_STAT_OK);
            else if (id == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0))
                pkInterp = ConvertTCBPoint3(pCol, pIKeys);
            else if ((id == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) ||
                     (id == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)))
                pkInterp = ConvertBezPoint3(pCol, pIKeys);
            else
                pkInterp = UnknownColAnim(pCol);
        }
        else
            pkInterp = UnknownColAnim(pCol);
    }
    else
        pkInterp = UnknownColAnim(pCol);

    return(pkInterp);
}

//---------------------------------------------------------------------------
//                  SCALE ANIMATION
//---------------------------------------------------------------------------

NiInterpolator* NiMAXAnimationConverter::ConvertTCBScale(Control *,
    IKeyControl *pIKeys, ScaleInfo *)
{
    NiTCBFloatKey *pKeys;
    float fFirstScale = 0.0f;
    int i, iNumKeys;

    iNumKeys = pIKeys->GetNumKeys();
    pKeys = NiNew NiTCBFloatKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    for (i = 0; i < iNumKeys; i++)
    {
        ITCBScaleKey tcb;
        float fScale;

        pIKeys->GetKey(i, &tcb);

        fScale = tcb.val.s.x * tcb.val.s.y * tcb.val.s.z;
        if (fScale < 0)
            fScale = (float) pow(-fScale, 0.3333333333f);
        else
            fScale = (float) pow(fScale, 0.3333333333f);

        // this may seem odd but the initial scale will be
        // pushed down onto the geometry - so we must rescale
        // the scale animation to always start at the identity
        if (i == 0)
        {
            if (fScale < 0.0001f)
            {
                fFirstScale = 1.0f;
                fScale = 0.0001f;
            }
            else
            {
                fFirstScale = fScale;
                fScale = 1.0f;
            }
        }
        else
        {
            NIASSERT(fFirstScale > 0.0001f);
            fScale /= fFirstScale;
        }
        // if fFirstScale == 0.0f then the geometry has
        // already been collapsed to a single point & none
        // of the later scaling will have any effect

        pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        pKeys[i].SetValue(fScale);
        pKeys[i].SetTension(tcb.tens);
        pKeys[i].SetContinuity(tcb.cont);
        pKeys[i].SetBias(tcb.bias);
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
    pkInterp->ReplaceKeys(pKeys, iNumKeys, NiFloatKey::TCBKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertBezScale(Control *, 
    IKeyControl *pIKeys, ScaleInfo *)
{
    NiBezFloatKey *pKeys;
    float fFirstScale = 0.0f;
    int i, iNumKeys;

    iNumKeys = pIKeys->GetNumKeys();
    pKeys = NiNew NiBezFloatKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    int iKeyIdx = 0;
    for (i = 0; i < iNumKeys; i++)
    {
        float fScale, fIn, fOut;
        IBezScaleKey bez;

        pIKeys->GetKey(i, &bez);

        if (bez.time > -2.4e7)
        {
            fScale = bez.val.s.x * bez.val.s.y * bez.val.s.z;
            if (fScale < 0)
                fScale = (float) pow(-fScale, 0.3333333333f);
            else
                fScale = (float) pow(fScale, 0.3333333333f);

            // this may seem odd but the initial scale will be
            // pushed down onto the geometry - so we must rescale
            // the scale animation to always start at the identity
            if (iKeyIdx == 0)
            {
                if (fScale < 0.0001f)
                {
                    fFirstScale = 1.0f;
                    fScale = 0.0001f;
                }
                else
                {
                    fFirstScale = fScale;
                    fScale = 1.0f;
                }
            }
            else
            {
                NIASSERT(fFirstScale > 0.0001f);
                fScale /= fFirstScale;
            }
            // if fFirstScale == 0.0f then the geometry has
            // already been collapsed to a single point & none
            // of the later scaling will have any effect

            fIn = bez.intan.x * bez.intan.y * bez.intan.z;
            if (fIn < 0)
                fIn = (float) -pow(-fIn, 0.3333333333f);
            else
                fIn = (float) pow(fIn, 0.3333333333f);

            fOut = bez.outtan.x * bez.outtan.y * bez.outtan.z;
            if (fOut < 0)
                fOut = (float) -pow(-fOut, 0.3333333333f);
            else
                fOut = (float) pow(fOut, 0.3333333333f);

            pKeys[iKeyIdx].SetTime(bez.time * SECONDSPERTICK);
            pKeys[iKeyIdx].SetValue(fScale);
            pKeys[iKeyIdx].SetInTan(fIn);
            pKeys[iKeyIdx].SetOutTan(fOut);
            iKeyIdx++;
        }
        else
        {
            char acString[512];
            NiSprintf(acString, 512, "Invalid scale key found at time %d, "
                "skipping that key!", (unsigned int) bez.time);
            NILOGWARNING(acString);
        }
    }

    iNumKeys = iKeyIdx;

    // We need to patch up the tangent vectors to match our interpolation
    // MAX's tangents are specified in ticks between samples and both 
    // vectors point into the segment. We convert this to 0..1 tangents
    // and the out tangent pointing away from the segment.
    if (iNumKeys)
    {
        float fLastTime, fCurTime, fNextTime, fTimeDelIn, fTimeDelOut;
        NiBezFloatKey *pCurKey, *pNextKey;
        float fIn, fOut;

        fLastTime = fCurTime = pKeys[0].GetTime() * TICKSPERSECOND;
        pCurKey = &pKeys[0];
        fTimeDelIn = fCurTime - fLastTime;

        for (i = 0; i < iNumKeys; i++)
        {
            if (i+1 < iNumKeys)
            {
                pNextKey = &pKeys[i+1];
                fNextTime = pNextKey->GetTime() * TICKSPERSECOND;
            }
            else
            {
                pNextKey = NULL;
                fNextTime = fCurTime;
            }

            fTimeDelOut = fNextTime - fCurTime;

            fIn = pCurKey->GetInTan();
            pCurKey->SetInTan(-fIn*fTimeDelIn);
            fOut = pCurKey->GetOutTan();
            pCurKey->SetOutTan(fOut*fTimeDelOut);

            fTimeDelIn = fTimeDelOut;
            fLastTime = fCurTime;
            fCurTime = fNextTime;
            pCurKey = pNextKey;
        }
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiFloatKey::BEZKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertLinScale(Control *,
    IKeyControl *pIKeys, ScaleInfo *)
{
    NiLinFloatKey *pKeys;
    float fFirstScale = 0.0f;
    int i, iNumKeys;

    iNumKeys = pIKeys->GetNumKeys();
    pKeys = NiNew NiLinFloatKey[iNumKeys];
    if (pKeys == NULL)
        return NULL;

    for (i = 0; i < iNumKeys; i++)
    {
        ILinScaleKey lin;
        float fScale;

        pIKeys->GetKey(i, &lin);

        fScale = lin.val.s.x * lin.val.s.y * lin.val.s.z;
        if (fScale < 0)
            fScale = (float) pow(-fScale, 0.3333333333f);
        else
            fScale = (float) pow(fScale, 0.3333333333f);

        // this may seem odd but the initial scale will be
        // pushed down onto the geometry - so we must rescale
        // the scale animation to always start at the identity
        if (i == 0)
        {
            if (fScale < 0.0001f)
            {
                fFirstScale = 1.0f;
                fScale = 0.0001f;
            }
            else
            {
                fFirstScale = fScale;
                fScale = 1.0f;
            }
        }
        else
        {
            NIASSERT(fFirstScale > 0.0001f);
            fScale /= fFirstScale;
        }
        // if fFirstScale == 0.0f then the geometry has
        // already been collapsed to a single point & none
        // of the later scaling will have any effect

        pKeys[i].SetTime(lin.time * SECONDSPERTICK);
        pKeys[i].SetValue(fScale);
    }

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
    pkInterp->SetKeys(pKeys, iNumKeys, NiFloatKey::LINKEY);
    return pkInterp;
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::ConvertDumbScale(Control* pControl,
    INode *pMaxNode, ScaleInfo *pParentScale)
{
    TimeValue animStart;
    TimeValue animEnd;
    unsigned int uiNumFrames;
    CalculateControlTimeRange(pControl, animStart, animEnd, uiNumFrames);

    NiLinFloatKey *pKeys = NULL;
    float fFirstScale = 0.0f;
    unsigned int i;
    TimeValue t;
    
    if (uiNumFrames == 0)
        return NULL;

    unsigned int uiMaxNumKeys = UINT_MAX / sizeof(NiLinFloatKey);
    if (uiNumFrames > uiMaxNumKeys)
    {
        // issue error message
        NiString strWarning = "The object \"";
        strWarning += (const char*)pMaxNode->GetName();
        strWarning += "\" Contains animation data too large to fit "
            "into memory. Adjust the time range of animations and re-export.";
        NILOGWARNING((const char*)strWarning);
        return NULL;
    }

    pKeys = NiNew NiLinFloatKey[uiNumFrames];
    if (pKeys == NULL)
        return NULL;

    float fLastScale = 0.0f;
    bool bAllSameValue = true;
    float fScale;

    for (i = 0, t = animStart; 
         i < uiNumFrames - 1; 
         i++, t += GetTicksPerFrame())
    {
        fScale = NiMAXHierConverter::GetLocalUScale(pMaxNode, t, pParentScale);

        // this may seem odd but the initial scale will be
        // pushed down onto the geometry - so we must rescale
        // the scale animation to always start at the identity
        if (i == 0)
        {
            if (fScale < 0.0001f)
            {
                fFirstScale = 1.0f;
                fScale = 0.0001f;
            }
            else
            {
                fFirstScale = fScale;
                fScale = 1.0f;
            }
        }
        else
        {
            NIASSERT(fFirstScale > 0.0001f);
            fScale /= fFirstScale;
        }
        // if fFirstScale == 0.0f then the geometry has
        // already been collapsed to a single point & none
        // of the later scaling will have any effect

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetValue(fScale);

        if (i != 0 && fScale != fLastScale)
            bAllSameValue = false;

        fLastScale = fScale;
    }
    // the last frame of the sample should be exact time, not rounded to
    // the nearest frame
    fScale = NiMAXHierConverter::GetLocalUScale(pMaxNode, animEnd, 
        pParentScale);
    fScale /= fFirstScale;
    pKeys[uiNumFrames - 1].SetTime(animEnd * SECONDSPERTICK);
    pKeys[uiNumFrames - 1].SetValue(fScale);

    if (fScale != fLastScale)
        bAllSameValue = false;

    if (!bAllSameValue)
    {
        NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
        pkInterp->SetKeys(pKeys, uiNumFrames, NiFloatKey::LINKEY);
        return pkInterp;
    }
    else
    {
        if (pKeys)
            NiDelete[] pKeys;
        return NULL;
    }
}

//---------------------------------------------------------------------------
NiInterpolator* NiMAXAnimationConverter::UnknownScaleAnim(Control *pSC,
    INode *pMaxNode, ScaleInfo *pParentScale)
{
    return ConvertDumbScale(pSC, pMaxNode, pParentScale);
}

//---------------------------------------------------------------------------
NiInterpolatorPtr NiMAXAnimationConverter::ConvertScaleAnim(Control *pScale, 
    NiTimeController::CycleType& eCycleType, INode *pMaxNode,
    ScaleInfo *pParentScale)
{
    IKeyControl *pIKeys;
    NiInterpolatorPtr spInterp;

    bool bSampleScaleKeys = false;
    if (pMaxNode->UserPropExists("NiSampleScaleKeys"))
    {
        bSampleScaleKeys = true;
    }

    if (pScale && !bSampleScaleKeys)
    {
        Control* pXc = pScale->GetXController();
        Control* pYc = pScale->GetYController();
        Control* pZc = pScale->GetZController();

        pIKeys = GetKeyControlInterface(pScale);

        if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() == 0)
                return NULL;
            else if (pScale->ClassID() ==
                     Class_ID(LININTERP_SCALE_CLASS_ID, 0))
                spInterp = ConvertLinScale(pScale, pIKeys, pParentScale);
            else if (pScale->ClassID() ==
                     Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
                spInterp = ConvertTCBScale(pScale, pIKeys, pParentScale);
            else if (pScale->ClassID() ==
                     Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0))
                spInterp = ConvertBezScale(pScale, pIKeys, pParentScale);
            else
                spInterp = UnknownScaleAnim(pScale, pMaxNode, pParentScale);
        }
        else if (pXc || pYc || pZc)
        {
            bool bSampleKeys = false;
            IKeyControl* pIXKeys = NULL;
            IKeyControl* pIYKeys = NULL;
            IKeyControl* pIZKeys = NULL;

            if (pXc)
            {
                 SetORT(pXc, eCycleType);
                 pIXKeys = GetKeyControlInterface(pXc);
                 if (pIXKeys && pIXKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIXKeys)
                     bSampleKeys = true;
            }

            if (pYc)
            {
                 SetORT(pYc, eCycleType);
                 pIYKeys = GetKeyControlInterface(pYc);
                 if (pIYKeys && pIYKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIYKeys)
                     bSampleKeys = true;
            }

            if (pZc)
            {
                 SetORT(pZc, eCycleType);
                 pIZKeys = GetKeyControlInterface(pZc);
                 if (pIZKeys && pIZKeys->GetNumKeys() > 0)
                     bSampleKeys = true;
                 else if (!pIZKeys)
                     bSampleKeys = true;
            }

            if (bSampleKeys)
            {
                spInterp = UnknownScaleAnim(pScale, pMaxNode, pParentScale);
            }
            else
            {
                return NULL;
            }
        }
        else
            spInterp = UnknownScaleAnim(pScale, pMaxNode, pParentScale);
    }
    else
        spInterp = UnknownScaleAnim(pScale, pMaxNode, pParentScale);
        
    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(NiFloatInterpolator, 
        spInterp);

    if (pkFloatInterp && pkFloatInterp->GetKeyCount(0) > 0)
    {
        pkFloatInterp->FillDerivedValues(0);
        SetORT(pScale, eCycleType);
    }

    return spInterp;
}

//---------------------------------------------------------------------------
//                  GLOBALLY ACCESSIBLE ROUTINES
//---------------------------------------------------------------------------
#ifndef NO_CSTUDIO
void NiMAXAnimationConverter::ConvertBipSlaveAnim(Animatable *pControl,
    INode *pMaxNode, ScaleInfo *pParentScale, unsigned int &uiNumKeys,
    NiLinPosKey **ppPosKeys, NiLinRotKey **ppRotKeys)
{
    // see bipexp.rtf for documentation on the limitations of
    // cs exporting

    Class_ID cID = pControl->ClassID();
    NIASSERT(cID == BIPSLAVE_CONTROL_CLASS_ID);

    // query MAX for the number of keyframes
    int iNumKeys = pControl->NumKeys();
    
    if (iNumKeys <= 0)
    {
        uiNumKeys = 0;
        *ppPosKeys = NULL;
        *ppRotKeys = NULL;
        return;
    }

    AffineParts parts;
    Matrix3 tm;
    NiPoint3 pos;
    NiQuaternion rot;
    NiLinPosKey *pkPosKeys;
    NiLinRotKey *pkRotKeys;
    unsigned char ucPosSize = sizeof (NiLinPosKey);
    unsigned char ucRotSize = sizeof (NiLinRotKey);
    
    pkPosKeys = NiNew NiLinPosKey[iNumKeys];
    NIASSERT(pkPosKeys);
    
    pkRotKeys = NiNew NiLinRotKey[iNumKeys];
    NIASSERT(pkRotKeys);
    
    for (int i = 0; i < iNumKeys; i++)
    {
        TimeValue t = pControl->GetKeyTime(i);
        
        // This is, sadly, all the information we can get about the
        // keyframe at this time
        tm = NiMAXHierConverter::GetLocalTM(pMaxNode, t, pParentScale);
        
        decomp_affine(tm, &parts);
        
        pos.x = parts.t.x;
        pos.y = parts.t.y;
        pos.z = parts.t.z;
        
        //
        // Use inverse quaternion to match 
        // Gamebryo rotation direction
        //
        rot.SetW(parts.q.w);
        rot.SetX(-parts.q.x);
        rot.SetY(-parts.q.y);
        rot.SetZ(-parts.q.z);
        
        // This is a TCB animation in MAX but we make it linear
        // here since we cannot extract the real TCB values.
        NiLinPosKey* pkLinPosKey = (NiLinPosKey*) pkPosKeys->GetKeyAt(i, 
            ucPosSize);
        pkLinPosKey->SetTime(t * SECONDSPERTICK);
        pkLinPosKey->SetPos(pos);

        NiLinRotKey* pkLinRotKey = (NiLinRotKey*) pkRotKeys->GetKeyAt(i, 
            ucRotSize);
        pkLinRotKey->SetTime(t * SECONDSPERTICK);
        pkLinRotKey->SetQuaternion(rot);
    }

    NiPosKey::FillDerivedValsFunction pPosDerived;
            
    pPosDerived = NiPosKey::GetFillDerivedFunction(NiPosKey::LINKEY);
    NIASSERT(pPosDerived);
            
    (*pPosDerived)((NiAnimationKey*)pkPosKeys, iNumKeys, ucPosSize);
            
    NiRotKey::FillDerivedValsFunction pRotDerived;
            
    pRotDerived = NiRotKey::GetFillDerivedFunction(NiRotKey::LINKEY);
    NIASSERT(pRotDerived);
            
    (*pRotDerived)((NiAnimationKey*)pkRotKeys, iNumKeys, ucRotSize);

    uiNumKeys = iNumKeys;
    *ppPosKeys = pkPosKeys;    
    *ppRotKeys = pkRotKeys;
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::MergeHorizontalAndVerticalAnimations(
    NiLinPosKey* pPos0, unsigned int ui0, NiLinPosKey* pPos1, 
    unsigned int ui1, NiLinPosKey*& pNewPos, unsigned int& uiNewPos)
{
    // combine pPos0 and pPos1.  Interpolated (x,y) positions
    // are already in the vertical track keys, as is the z position
    // in the horizontal track keys.  So all we need to do is create a key
    // at each time there is a key in either track.

    unsigned char ucSize = sizeof(NiLinPosKey);
    NiTList<NiLinPosKey *> keylist;

    unsigned int uiJStart = 0;
    for (unsigned int i = 0; i < ui1; i++)
    {
        // check time of each key in ppPos1
        float fTime1 = pPos1->GetKeyAt(i, ucSize)->GetTime();

        // find a key in ppPos0 with the same time, if any
        for (unsigned int j = uiJStart; j < ui0; j++)
        {
            float fTime0 = pPos0->GetKeyAt(j, ucSize)->GetTime();

            if (fTime0 > fTime1)
            {
                // didn't find one; can stop looking; will need a key here
                // at fTime1. 
                NiLinPosKey* key = NiNew NiLinPosKey(fTime1,
                    pPos1->GetKeyAt(i, ucSize)->GetPos());
                keylist.AddTail(key);
                break;
            }

            // can start searching here next time
            uiJStart++;

            if (fTime0 == fTime1)
            {
                // found one, can use either key
                NiLinPosKey* key = NiNew NiLinPosKey(fTime0,
                    pPos1->GetKeyAt(i, ucSize)->GetPos());
                keylist.AddTail(key);
                break;
            }
            
            // assert: (fTime0 < fTime1)
            {

                // havent found one yet, but instead found an additional key
                // in ppPos0 at fTime0.
                NiLinPosKey* key = NiNew NiLinPosKey(fTime0,
                    pPos0->GetKeyAt(j, ucSize)->GetPos());
                keylist.AddTail(key);
            }
        }
    }
    
    uiNewPos = keylist.GetSize();
    pNewPos = NiNew NiLinPosKey[uiNewPos];
    NIASSERT(pNewPos);

    NiTListIterator pIter = keylist.GetHeadPos();
    unsigned int uiIndex = 0;
    while(pIter)
    {
        NiLinPosKey* pPosKey = keylist.GetNext(pIter);
        NIASSERT(uiIndex < uiNewPos);
        NIASSERT(pPosKey);
        NiLinPosKey* pkNewLinPosKey = (NiLinPosKey*)
            pNewPos->GetKeyAt(uiIndex, ucSize);
        pkNewLinPosKey->SetTime(pPosKey->GetTime());
        pkNewLinPosKey->SetPos(pPosKey->GetPos());
        NiDelete pPosKey;
        uiIndex++;
    }

}
#endif
//---------------------------------------------------------------------------
NiTransformInterpolator*  NiMAXAnimationConverter::CreateTransformInterp(
    NiInterpolator* pkTranslate, NiInterpolator* pkRotate, 
    NiInterpolator* pkScale)
{ 
    if (!pkTranslate && ! pkRotate && !pkScale)
        return NULL;

    NiTransformInterpolator* pkTransformInterp = NiNew
        NiTransformInterpolator;
    
    if (pkTranslate)
    {
        NiPoint3Interpolator* pkKeyInterp = NiDynamicCast(
                NiPoint3Interpolator, pkTranslate);
            
        if (pkKeyInterp)
        {
            unsigned int uiNumKeys;
            unsigned char ucSize;
            NiAnimationKey::KeyType eType;
            NiPosKey* pkKeys = (NiPosKey*)pkKeyInterp->GetAnim(uiNumKeys,
                eType, ucSize, 0);
            
            if (pkKeyInterp->GetChannelPosed(0))
            {
                NiPoint3 kValue;
                if (pkKeyInterp->Update(0.0f, NULL, kValue))
                    pkTransformInterp->SetPoseTranslate(kValue);
            }
            else if (uiNumKeys)
            {
                // Transfer ownership of keys
                pkKeyInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
                pkTransformInterp->ReplacePosData(pkKeys, uiNumKeys,
                    eType);
            }
        }
    }

    if (pkRotate)
    {
        NiQuaternionInterpolator* pkKeyInterp = NiDynamicCast(
            NiQuaternionInterpolator, pkRotate);
        
        if (pkKeyInterp)
        {
            unsigned int uiNumKeys;
            unsigned char ucSize;
            NiAnimationKey::KeyType eType;
            NiRotKey* pkKeys = (NiRotKey*)pkKeyInterp->GetAnim(uiNumKeys,
                eType, ucSize, 0);

            if (pkKeyInterp->GetChannelPosed(0))
            {
                NiQuaternion kValue;
                if (pkKeyInterp->Update(0.0f, NULL, kValue))
                    pkTransformInterp->SetPoseRotate(kValue);
            }
            else if (uiNumKeys)
            {
                // Transfer ownership of keys
                pkKeyInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
                pkTransformInterp->ReplaceRotData(pkKeys, uiNumKeys,
                    eType);
            }
        }
    }

    if (pkScale)
    {
        NiFloatInterpolator* pkKeyInterp = NiDynamicCast(
            NiFloatInterpolator, pkScale);
        
        if (pkKeyInterp)
        {
            unsigned int uiNumKeys;
            unsigned char ucSize;
            NiAnimationKey::KeyType eType;
            NiFloatKey* pkKeys = (NiFloatKey*)pkKeyInterp->GetAnim(uiNumKeys,
                eType, ucSize, 0);
            if (pkKeyInterp->GetChannelPosed(0))
            {
                float fValue;
                if (pkKeyInterp->Update(0.0f, NULL, fValue))
                    pkTransformInterp->SetPoseScale(fValue);
            }
            else if (uiNumKeys)
            {
                // Transfer ownership of keys
                pkKeyInterp->SetKeys(0, 0, NiAnimationKey::NOINTERP);
                pkTransformInterp->ReplaceScaleData(pkKeys, uiNumKeys,
                    eType);
            }
        }
    }

    if (pkTransformInterp->GetNumPosKeys() != 0 ||
        pkTransformInterp->GetNumRotKeys() != 0 ||
        pkTransformInterp->GetNumScaleKeys() != 0 ||
        pkTransformInterp->GetChannelPosed(
        NiTransformInterpolator::POSITION) ||
        pkTransformInterp->GetChannelPosed(
        NiTransformInterpolator::ROTATION) ||
        pkTransformInterp->GetChannelPosed(NiTransformInterpolator::SCALE))
    {
        return pkTransformInterp;
    }
    else
    {
        NiDelete pkTransformInterp;
        return NULL;
    }
}
//---------------------------------------------------------------------------
// convert any animation attached to "pMaxNode" and add it to "pNode"
int NiMAXAnimationConverter::Convert(NiNode *pNode, INode *pMaxNode, 
    ScaleInfo *pParentScale, bool bIgnoreVisAnim)
{
    Control *pPC, *pRC, *pSC, *pVC, *pControl;
    Class_ID cID; 
    int iStatus;

    pMaxNode->GetName();
    pControl = pMaxNode->GetTMController();
    pVC = pMaxNode->GetVisController();
    if (!pControl && !pVC)
        return(W3D_STAT_OK);

    if (pControl)
    {
        pPC = pControl->GetPositionController();
        pRC = pControl->GetRotationController();
        pSC = pControl->GetScaleController();
        cID = pControl->ClassID();
    }
    else
    {
        pPC = pRC = pSC = NULL;
        cID = Class_ID(0, 0);
    }
#ifndef NO_CSTUDIO
    if (ms_bConvertCSAsKeyframe && cID == BIPBODY_CONTROL_CLASS_ID)
    {
        Animatable *pVert, *pHoriz, *pRot;

        NiLinPosKey *pPos0, *pPos1, *pPos2;
        NiLinRotKey *pRot0, *pRot1, *pRot2;
        unsigned int ui0, ui1, ui2;

        // This 3ds max Plug-in may not be able to convert all the animation
        //    on the biped body without sampling.  In particular, the
        //    following are not exposed in the Biped api:
        //      -  Forward & Inverse Kinematics blending for the hands & feet,
        //      -  Collision detection and response for footsteps, and
        //      -  Physically based interpolation for the vertical track of
        //            the COM.
        // Freeform Animation (spline based interpolation of the COM), in
        //    which the animator:
        //      -  Does not use IK Blend to effect the interpolation of the
        //            hands or feet, and
        //      -  Does not change the TCB settings from their defaults,
        //    should export fairly accurately.

        pVert = pControl->SubAnim(VERTICAL_SUBANIM);
        NIASSERT(pVert->ClassID() == BIPSLAVE_CONTROL_CLASS_ID);

        ConvertBipSlaveAnim(pVert, pMaxNode, pParentScale,
                            ui0, &pPos0, &pRot0);

        pHoriz = pControl->SubAnim(HORIZONTAL_SUBANIM);
        NIASSERT(pHoriz->ClassID() == BIPSLAVE_CONTROL_CLASS_ID);

        ConvertBipSlaveAnim(pHoriz, pMaxNode, pParentScale,
                            ui1, &pPos1, &pRot1);

        pRot = pControl->SubAnim(ROTATION_SUBANIM);
        NIASSERT(pRot->ClassID() == BIPSLAVE_CONTROL_CLASS_ID);

        ConvertBipSlaveAnim(pRot, pMaxNode, pParentScale,
                            ui2, &pPos2, &pRot2);

        if (ui0 > 0 || ui1 > 0 || ui2 > 0)
        {
            NiTransformController *pkKFController;

            pkKFController = NiNew NiTransformController;
            NIASSERT(pkKFController);
            
            if (NiMAXConverter::GetUseAppTime())
                pkKFController->SetAnimType(NiTimeController::APP_TIME);
            else
                pkKFController->SetAnimType(NiTimeController::APP_INIT);
            
            pkKFController->SetTarget(pNode);

            // merge horizontal and vertical tracks
            NiLinPosKey* pNewPos;
            unsigned int uiNewPos;

            MergeHorizontalAndVerticalAnimations(pPos0, ui0, pPos1, ui1,
                pNewPos, uiNewPos);

            NiTransformInterpolator* pkInterp = NiNew 
                NiTransformInterpolator;

            pkInterp->ReplacePosData(pNewPos, uiNewPos,
                NiPosKey::LINKEY);
            pkInterp->ReplaceRotData(pRot2, ui2, NiRotKey::LINKEY);
            pkKFController->SetInterpolator(pkInterp);
            pkKFController->ResetTimeExtrema();
            pkInterp->Collapse();
            
            NiDelete [] pRot0;

            NiDelete [] pPos1;
            NiDelete [] pRot1;

            NiDelete [] pPos2;

            SetORT(pControl, pkKFController);
        }

        return(W3D_STAT_OK);
    }
    else if (ms_bConvertCSAsKeyframe && cID == BIPSLAVE_CONTROL_CLASS_ID)
    {
        NiLinPosKey *pkPosKeys;
        NiLinRotKey *pkRotKeys;
        unsigned int uiNumKeys;

        ConvertBipSlaveAnim(pControl, pMaxNode, pParentScale,
                            uiNumKeys, &pkPosKeys, &pkRotKeys);
        
        if (uiNumKeys > 0)
        {
            NiTransformController *pkKFController;
            
            pkKFController = NiNew NiTransformController;
            NIASSERT(pkKFController);
            
            if (NiMAXConverter::GetUseAppTime())
                pkKFController->SetAnimType(NiTimeController::APP_TIME);
            else
                pkKFController->SetAnimType(NiTimeController::APP_INIT);
            
            pkKFController->SetTarget(pNode);
            
            NiTransformInterpolator* pkInterp = NiNew 
                NiTransformInterpolator;

            pkInterp->ReplacePosData(pkPosKeys, 
                                          uiNumKeys, 
                                          NiPosKey::LINKEY);
            pkInterp->ReplaceRotData(pkRotKeys, 
                                          uiNumKeys, 
                                          NiRotKey::LINKEY);

            pkKFController->SetInterpolator(pkInterp);
            pkKFController->ResetTimeExtrema();
            pkInterp->Collapse();
            SetORT(pControl, pkKFController);
        }

        // all animations will be subsumed into this one so don't allow
        // any other controllers on this node. If other controllers are
        // allowed then some animations will be applied twice.
        return(W3D_STAT_OK);
    }
    else if (cID == FOOTPRINT_CLASS_ID)
    {
        // we don't want to pay for any animation on the footsteps
        return(W3D_STAT_OK);
    }
    else if (cID == BIPBODY_CONTROL_CLASS_ID || 
             cID == BIPSLAVE_CONTROL_CLASS_ID)
    {
        NiTransformController *pkKFController;
        PosRotAnimInfo *pTmp;

        // need to add this node to the PosRot anim list so that
        // its animation will be converted in the Finish routine
        pkKFController = NiNew NiTransformController;
        NIASSERT(pkKFController);
        
        if (NiMAXConverter::GetUseAppTime())
            pkKFController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkKFController->SetAnimType(NiTimeController::APP_INIT);

        pkKFController->SetTarget(pNode);

        pTmp = NiNew PosRotAnimInfo;
        NIASSERT(pTmp);

        NiTransformInterpolator* pkInterp = NiNew NiTransformInterpolator;
        pTmp->pTransformController = pkKFController;
        pTmp->pTransformInterp = pkInterp;
        pTmp->pMaxNode = pMaxNode;
        pTmp->ParentScale = *pParentScale;

        // go ahead and allocate space for the keys (one per frame)
        if (m_uiNumFrames)
        {
            pTmp->pkRotKeys = (NiRotKey*) NiNew NiLinRotKey[m_uiNumFrames];
            pTmp->pkPosKeys = (NiPosKey*) NiNew NiLinPosKey[m_uiNumFrames];
        }
        else
        {
            pTmp->pkRotKeys = 0;
            pTmp->pkPosKeys = 0;
        }

        ms_PosRotAnimList.AddHead(pTmp);

        // all animations will be subsumed into this one so don't allow
        // any other controllers on this node. If other controllers are
        // allowed then some animations will be applied twice.
        return(W3D_STAT_OK);
    }
#endif
    
    if (cID == IKCONTROL_CLASS_ID || cID == IKCHAINCONTROL_CLASS_ID )
    {
        NiTransformController *pkKFController;
        PosRotAnimInfo *pTmp;

        // need to add this node to the PosRot anim list so that
        // its animation will be converted in the Finish routine
        pkKFController = NiNew NiTransformController;
        NIASSERT(pkKFController);
        
        if (NiMAXConverter::GetUseAppTime())
            pkKFController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkKFController->SetAnimType(NiTimeController::APP_INIT);

        pkKFController->SetTarget(pNode);

        pTmp = NiNew PosRotAnimInfo;
        NIASSERT(pTmp);

        NiTransformInterpolator* pkInterp = NiNew NiTransformInterpolator;
        pTmp->pTransformController = pkKFController;
        pTmp->pTransformInterp = pkInterp;
        pTmp->pMaxNode = pMaxNode;
        pTmp->ParentScale = *pParentScale;

        // go ahead and allocate space for the keys (one per frame)
        if (m_uiNumFrames)
        {
            pTmp->pkRotKeys = (NiRotKey*) NiNew NiLinRotKey[m_uiNumFrames];
            pTmp->pkPosKeys = (NiPosKey*) NiNew NiLinPosKey[m_uiNumFrames];
        }
        else
        {
            pTmp->pkRotKeys = 0;
            pTmp->pkPosKeys = 0;
        }

        ms_PosRotAnimList.AddHead(pTmp);

        // all animations will be subsumed into this one so don't allow
        // any other controllers on this node. If other controllers are
        // allowed then some animations will be applied twice.
        return(W3D_STAT_OK);
    }
    if (cID == IKSLAVE_CLASSID )
    {
        NiTransformController *pkKFController;
        PosRotAnimInfo *pTmp;

        // need to add this node to the PosRot anim list so that
        // its animation will be converted in the Finish routine
        pkKFController = NiNew NiTransformController;
        NIASSERT(pkKFController);
        
        if (NiMAXConverter::GetUseAppTime())
            pkKFController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkKFController->SetAnimType(NiTimeController::APP_INIT);

        pkKFController->SetTarget(pNode);

        pTmp = NiNew PosRotAnimInfo;
        NIASSERT(pTmp);

        NiTransformInterpolator* pkInterp = NiNew NiTransformInterpolator;
        pTmp->pTransformController = pkKFController;
        pTmp->pTransformInterp = pkInterp;
        pTmp->pMaxNode = pMaxNode;
        pTmp->ParentScale = *pParentScale;

        // go ahead and allocate space for the keys (one per frame)
        if (m_uiNumFrames)
        {
            pTmp->pkRotKeys = (NiRotKey*) NiNew NiLinRotKey[m_uiNumFrames];
            pTmp->pkPosKeys = (NiPosKey*) NiNew NiLinPosKey[m_uiNumFrames];
        }
        else
        {
            pTmp->pkRotKeys = 0;
            pTmp->pkPosKeys = 0;
        }

        ms_PosRotAnimList.AddHead(pTmp);

        // all animations will be subsumed into this one so don't allow
        // any other controllers on this node. If other controllers are
        // allowed then some animations will be applied twice.
        return(W3D_STAT_OK);
    }

    NiLookAtInterpolator* pkLookAtInterp = NULL;
    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    
    if (cID == Class_ID(LOOKAT_CONTROL_CLASS_ID, 0))
    {
        pkLookAtInterp = ConvertLookAt(pControl, eCycleType);
        pRC = NULL;
    }

    Control* pkPathCont = GetPathInterpolator(pMaxNode);
    if (pkPathCont)
    {
        NiTransformControllerPtr spTransformControl = 
            NiNew NiTransformController;
        if (NiMAXConverter::GetUseAppTime())
            spTransformControl->SetAnimType(NiTimeController::APP_TIME);
        else
            spTransformControl->SetAnimType(NiTimeController::APP_INIT);

        NiPathInterpolator *pPathInterpolator = NiNew NiPathInterpolator;
        NIASSERT(pPathInterpolator);

        iStatus = ConvertPath(pMaxNode, pkPathCont, pPathInterpolator, 
            eCycleType, pParentScale);

        if (iStatus)
        {
            char acString[1024];
            NiSprintf(acString, 1024, "Failed to convert path control on "
                "\"%s\"\n", pMaxNode->GetName());
            NILOGWARNING(acString);
            return W3D_STAT_OK;
        }

        spTransformControl->SetInterpolator(pPathInterpolator);
        spTransformControl->ResetTimeExtrema();
        spTransformControl->SetTarget(pNode);
        spTransformControl->SetCycleType(eCycleType);
    }
    else if (pPC || pRC || pSC || pkLookAtInterp)
    {
        NiTransformController *pkKFController;

        pkKFController = NiNew NiTransformController;
        NIASSERT(pkKFController);

        if (NiMAXConverter::GetUseAppTime())
            pkKFController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkKFController->SetAnimType(NiTimeController::APP_INIT);

        NiInterpolatorPtr spFinalTransformInterp = NULL;

        NiInterpolatorPtr spTranslateInterp = NULL;
        NiInterpolatorPtr spRotateInterp = NULL;
        NiInterpolatorPtr spScaleInterp = NULL;

        if (pPC)
        {
            bool bUnused;
            spTranslateInterp = ConvertPositionAnim(pPC, eCycleType, 
                pMaxNode, pParentScale, bUnused);
        }

        if (pRC)
        {
            spRotateInterp = ConvertRotationAnim(pRC, eCycleType, 
                pMaxNode, pParentScale);
        }

        if (pSC)
        {
            spScaleInterp = ConvertScaleAnim(pSC, eCycleType,
                pMaxNode, pParentScale);
        }

        if (pkLookAtInterp)
        {
            if (spTranslateInterp || spScaleInterp)
            {
                pkLookAtInterp->SetTranslateInterpolator(spTranslateInterp);
                pkLookAtInterp->SetScaleInterpolator(spScaleInterp);
            }

            float fStart;
            float fEnd;

            pkLookAtInterp->GetActiveTimeRange(fStart, fEnd);
            pkLookAtInterp->GuaranteeTimeRange(fStart, fEnd);

            spFinalTransformInterp = pkLookAtInterp;
        }
        else 
        {
            spFinalTransformInterp = CreateTransformInterp(
                spTranslateInterp, spRotateInterp, spScaleInterp);
        }

        if (spFinalTransformInterp)
        {
            pkKFController->SetInterpolator(spFinalTransformInterp);
            pkKFController->ResetTimeExtrema();
            pkKFController->SetTarget(pNode);
            pkKFController->SetCycleType(eCycleType);
            spFinalTransformInterp->Collapse();
        }
        else
        {
            NiDelete pkKFController;
        }
    }
    else
    {
        // if a transform controller exists, but the pos, rot, scale,
        // path, and lookat don't, then we just sample the transform.

        NiTransformController *pkKFController = NiNew NiTransformController;
        NIASSERT(pkKFController);

        if (NiMAXConverter::GetUseAppTime())
            pkKFController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkKFController->SetAnimType(NiTimeController::APP_INIT);

        NiInterpolatorPtr spTranslateInterp = NULL;
        NiInterpolatorPtr spRotateInterp = NULL;
        NiInterpolatorPtr spScaleInterp = NULL;

        NiInterpolatorPtr spFinalTransformInterp = NULL;

        spTranslateInterp = UnknownPosAnim(pPC, pMaxNode, pParentScale);
        spRotateInterp = UnknownRotAnim(pRC, pMaxNode, pParentScale);
        spScaleInterp = UnknownScaleAnim(pSC, pMaxNode, pParentScale);

        spFinalTransformInterp = CreateTransformInterp(
                spTranslateInterp, spRotateInterp, spScaleInterp);

        if (spFinalTransformInterp)
        {
            pkKFController->SetInterpolator(spFinalTransformInterp);
            pkKFController->ResetTimeExtrema();
            pkKFController->SetTarget(pNode);
            pkKFController->SetCycleType(eCycleType);
            spFinalTransformInterp->Collapse();
        }
        else
        {
            NiDelete pkKFController;
        }
    }

    if (pVC && !bIgnoreVisAnim)
    {
        NiVisController *pVisController;

        pVisController = NiNew NiVisController;
        NIASSERT(pVisController);
        
        if (NiMAXConverter::GetUseAppTime())
            pVisController->SetAnimType(NiTimeController::APP_TIME);
        else
            pVisController->SetAnimType(NiTimeController::APP_INIT);

        iStatus = ConvertVisAnim(pVC, pVisController);
        
        if (iStatus)
        {
            char acString[1024];
            NiSprintf(acString, 1024, "Failed to convert visibility control "
                "on \"%s\"\n", pMaxNode->GetName());
            NILOGWARNING(acString);
            return W3D_STAT_OK;
        }

        if (pVisController->GetInterpolator(0) != NULL)
            pVisController->SetTarget(pNode);
        else
            NiDelete pVisController;
    }

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::Init()
{
    NIASSERT(ms_lookAtList.IsEmpty());
    NIASSERT(ms_PosRotAnimList.IsEmpty());
}
//---------------------------------------------------------------------------
void NiMAXAnimationConverter::Shutdown()
{
    NiTPointerList<LookAtInfo *> *pLookAt;

    pLookAt = &ms_lookAtList;
    NiTListIterator kIter = pLookAt->GetHeadPos();
    while (kIter)
    {
        NiDelete pLookAt->RemovePos(kIter);
    }
    
    ms_lookAtList.RemoveAll();
    ms_PosRotAnimList.RemoveAll();
}
//---------------------------------------------------------------------------
int NiMAXAnimationConverter::Finish()
{
    if (ms_lookAtList.GetHeadPos() != NULL)
    {
        NiTPointerList<LookAtInfo *> *pList = &ms_lookAtList;

        NiTListIterator kIter = pList->GetHeadPos();
        while (kIter)
        {
            LookAtInfo *pLookAt;
            NiNode *pRetNode;

            pLookAt = pList->GetNext(kIter);

            pRetNode = NiMAXHierConverter::Lookup(pLookAt->pLookAt, NULL);
            if (pRetNode)
            {
                // We have already built this node - so just refer to it
                pLookAt->pControl->SetLookAt(pRetNode);
            }
            else
            {
                NILOGWARNING("LookAt Target was not set because target"
                    " was not exported\n");
            }
        }
    }

    // Sample Biped animation (done if export sampled Biped keyframes 
    // is selected)

#ifndef NO_CSTUDIO
    int iNumAnims = ms_PosRotAnimList.GetSize();
    if (iNumAnims == 0)
        return(W3D_STAT_OK);

    if (m_uiNumFrames == 0)
        return(W3D_STAT_OK);

    // loop over every frame of the animation
    // Note that these two loops (frames/nodes) must occur in this
    // order. If the order is reversed conversion will slow to a crawl
    TimeValue t;
    unsigned int i;
    NiTPointerList<PosRotAnimInfo *> *pEle;
    NiTListIterator kIter;
    for (i = 0, t = m_animStart; 
        i < m_uiNumFrames; 
        i++, t += GetTicksPerFrame())
    {
        AffineParts parts;
        Matrix3 tm;
        NiPoint3 pos;
        NiQuaternion rot;

        // Loop over every node that had either a BIPSLAVE or BIPBODY
        // controller
        for (pEle = &ms_PosRotAnimList, kIter = pEle->GetHeadPos();
            kIter; )
        {
            PosRotAnimInfo *pkAnim = pEle->GetNext(kIter);

            tm = NiMAXHierConverter::GetLocalTM(pkAnim->pMaxNode, t, 
                &(pkAnim->ParentScale));

            decomp_affine(tm, &parts);

            pos.x = parts.t.x;
            pos.y = parts.t.y;
            pos.z = parts.t.z;

            //
            // Use inverse quaternion to match 
            // Gamebryo rotation direction
            //
            rot.SetW(parts.q.w);
            rot.SetX(-parts.q.x);
            rot.SetY(-parts.q.y);
            rot.SetZ(-parts.q.z);

            NiLinPosKey* pkLinPosKey = (NiLinPosKey*)
                pkAnim->pkPosKeys->GetKeyAt(i, sizeof(NiLinPosKey));
            pkLinPosKey->SetTime(t * SECONDSPERTICK);
            pkLinPosKey->SetPos(pos);

            NiLinRotKey* pkLinRotKey = (NiLinRotKey*)
                pkAnim->pkRotKeys->GetKeyAt(i,
                sizeof(NiLinRotKey));
            pkLinRotKey->SetTime(t * SECONDSPERTICK);
            pkLinRotKey->SetQuaternion(rot);
        }
    }

    NiPosKey::FillDerivedValsFunction kDerivePos;
    kDerivePos = NiPosKey::GetFillDerivedFunction(NiPosKey::LINKEY);
    NIASSERT(kDerivePos);
    NiRotKey::FillDerivedValsFunction kDeriveRot;
    kDeriveRot = NiRotKey::GetFillDerivedFunction(NiRotKey::LINKEY);
    NIASSERT(kDeriveRot);

    // At this point we have one key per frame.  Reduce
    // keyframes with NiOptimize.
    NiTListIterator kIter2 = ms_PosRotAnimList.GetHeadPos();
    for (pEle = &ms_PosRotAnimList; kIter2 != NULL; )
    {
        PosRotAnimInfo *pkAnim = pEle->GetNext(kIter2);

        // first fill derived values

        (*kDerivePos)((NiAnimationKey*)pkAnim->pkPosKeys, m_uiNumFrames,
            sizeof(NiLinPosKey));
        (*kDeriveRot)((NiAnimationKey*)pkAnim->pkRotKeys, m_uiNumFrames,
            sizeof(NiLinRotKey));

        pkAnim->pTransformInterp->ReplacePosData(pkAnim->pkPosKeys,
            m_uiNumFrames, NiAnimationKey::LINKEY);
        pkAnim->pTransformInterp->ReplaceRotData(pkAnim->pkRotKeys,
            m_uiNumFrames, NiAnimationKey::LINKEY);
        pkAnim->pTransformController->SetInterpolator(
            pkAnim->pTransformInterp);
        pkAnim->pTransformController->ResetTimeExtrema();
        pkAnim->pTransformInterp->Collapse();
    }

    kIter2 = ms_PosRotAnimList.GetHeadPos();
    for (pEle = &ms_PosRotAnimList; kIter2 != NULL;)
    {
        NiDelete pEle->RemovePos(kIter2);
    }

    ms_PosRotAnimList.RemoveAll();
#endif

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
Control* NiMAXAnimationConverter::GetPathInterpolator(INode *pMaxNode)
{
    Control *pCont;

    pCont = pMaxNode->GetTMController();
    if (pCont)
    {
        Control *pPosCont;

        pPosCont = pCont->GetPositionController();

        if (pPosCont == NULL)
            return NULL;

        if (pPosCont->ClassID() == Class_ID(PATH_CONTROL_CLASS_ID, 0))
                return (IPathPosition*) pPosCont;
        IListControl* pIListCtrl = GetIListControlInterface(pPosCont);
        if (pIListCtrl)
        {
            ListControl* pkListControl = (ListControl*) pIListCtrl;
            unsigned int uiSubCtrls = pkListControl->conts.Count();


            if (uiSubCtrls == 1 || uiSubCtrls == 2)
            {
                int iActive = pIListCtrl->GetActive();
                pPosCont = (Control*) pIListCtrl->SubAnim(iActive);
                if (pPosCont && pPosCont->ClassID() == 
                    Class_ID(PATH_CONTROL_CLASS_ID, 0))
                {
                    return (IPathPosition*) pPosCont;
                }
            }
        
        }
    }

    return NULL;
}

//---------------------------------------------------------------------------
bool NiMAXAnimationConverter::IsIKControlled(INode *pMaxNode)
{
    Control *pCont = pMaxNode->GetTMController();

    if (!pCont)
        return false;

    if (pCont->ClassID() == IKSLAVE_CLASSID ||
        (pCont->GetPositionController() &&
         pCont->GetPositionController()->ClassID()==IKSLAVE_CLASSID) ||
        (pCont->GetRotationController() &&
         pCont->GetRotationController()->ClassID()==IKSLAVE_CLASSID))
         return (true);

    return(false);
}

//---------------------------------------------------------------------------
NiMAXAnimationConverter::NiMAXAnimationConverter(TimeValue animStart, 
    TimeValue animEnd)
{ 
    m_animStart = animStart; 
    m_animEnd = animEnd; 
    m_uiNumFrames = ((m_animEnd - m_animStart) / GetTicksPerFrame()) + 1;
}

//---------------------------------------------------------------------------
void NiMAXAnimationConverter::CalculateControlTimeRange(Control* pControl,
    TimeValue& animStart, TimeValue& animEnd, unsigned int& uiNumFrames)
{
    TimeValue tempStart = TIME_PosInfinity;
    TimeValue tempEnd = TIME_NegInfinity;
    CalculateAnimatableTimeRange(pControl, tempStart, tempEnd);

    if (tempStart != TIME_NegInfinity && tempStart != TIME_PosInfinity)
        animStart = tempStart;
    else
        animStart = m_animStart;

    if (tempEnd != TIME_NegInfinity && tempEnd != TIME_PosInfinity)
        animEnd = tempEnd;
    else
        animEnd = m_animEnd;

    uiNumFrames = ((animEnd - animStart) / GetTicksPerFrame()) + 1;
}
//---------------------------------------------------------------------------
void NiMAXAnimationConverter::CalculateGlobalTimeRange( 
    TimeValue& animStart, TimeValue& animEnd)
{
    Interface* pkInterface = GetCOREInterface();
    INode* pkNode = pkInterface->GetRootNode();

    CalculateNodeTimeRange(pkNode, animStart, animEnd);
    CalculateAnimatableTimeRange(pkNode, animStart, animEnd);
}
//---------------------------------------------------------------------------
void NiMAXAnimationConverter::CalculateAnimatableTimeRange(
    Animatable* pkAnim, TimeValue& animStart, TimeValue& animEnd)
{
    if (pkAnim == NULL)
        return;

    Interval kInterval = pkAnim->GetTimeRange(TIMERANGE_ALL | 
        TIMERANGE_CHILDNODES | TIMERANGE_CHILDANIMS);

    bool bFoundStartRange = false;
    bool bFoundEndRange = false;
    TimeValue kNewStart = kInterval.Start();
    TimeValue kNewEnd =  kInterval.End();
    if (kNewStart == kNewEnd)
        return;

    if (kNewStart != TIME_NegInfinity && kNewStart != TIME_PosInfinity)
    {
        bFoundStartRange = true;
        if (animStart > kNewStart)
            animStart = kNewStart;
    }

    if (kNewEnd != TIME_PosInfinity && kNewEnd != TIME_NegInfinity)
    {
        bFoundEndRange = true;
        if (animEnd < kNewEnd)
            animEnd = kNewEnd;
    }
}
//---------------------------------------------------------------------------
void NiMAXAnimationConverter::CalculateNodeTimeRange(INode* pkAnim, 
    TimeValue& animStart, TimeValue& animEnd)
{
    if (pkAnim == NULL)
        return;

    if (pkAnim->IsAnimated())
    {
    }

    if (pkAnim->HasNoteTracks())
    {
        for (unsigned int ui = 0; ui < 
            (unsigned int) pkAnim->NumNoteTracks(); ui++)
        {
            DefNoteTrack * notetrack = 
                (DefNoteTrack *)pkAnim->GetNoteTrack(ui);
            if(notetrack) 
            {
                for(int k = 0; k < notetrack->keys.Count();k++)
                {
                    NoteKey * notekey = *(notetrack->keys.Addr(k));
                    if (notekey)
                    {
                        if (notekey->time < animStart)
                            animStart = notekey->time;
                        if (notekey->time > animEnd)
                            animEnd= notekey->time;
                    }
                }
            }

        }
    }

    for (unsigned int un = 0; un < (unsigned int) pkAnim->NumberOfChildren(); 
        un++)
    {
        INode* pkChild = pkAnim->GetChildNode(un);
        CalculateNodeTimeRange(pkChild, animStart, animEnd);
    }
}
//---------------------------------------------------------------------------
NiTextKeyExtraData* NiMAXAnimationConverter::BuildNoteTrack(INode* pMaxNode)
{
    if (pMaxNode->HasNoteTracks())
    {
        int iNumNoteTracks = pMaxNode->NumNoteTracks();
        int i = 0;
        unsigned int uiTotalNoteKeys = 0;
        // Calculate the total # of keys we need
        for (i = 0; i < iNumNoteTracks; i++)
        {
            Class_ID cID;

            NoteTrack* pNoteTrack = pMaxNode->GetNoteTrack(i);
            cID = pNoteTrack->ClassID();

            if (cID == Class_ID(NOTETRACK_CLASS_ID, 0))
            {
                DefNoteTrack *pNotes;

                pNotes = (DefNoteTrack *) pNoteTrack;
                uiTotalNoteKeys +=pNotes->NumKeys();
            }
        }

        unsigned int uiInsertedKeys = 0;

        // Build the key array in order, this means an insertion sort
        if (uiTotalNoteKeys)
        {
            float* pfTimes = NiAlloc(float, uiTotalNoteKeys);
            char** ppcNotes = NiAlloc(char*, uiTotalNoteKeys);

            for (i = 0; i < iNumNoteTracks; i++)
            {
                Class_ID cID;

                NoteTrack* pNoteTrack = pMaxNode->GetNoteTrack(i);
                cID = pNoteTrack->ClassID();

                if (cID == Class_ID(NOTETRACK_CLASS_ID, 0))
                {
                    DefNoteTrack *pNotes;

                    pNotes = (DefNoteTrack *) pNoteTrack;
                    if (pNotes->NumKeys())
                    {
                        for (int j = 0; j < pNotes->NumKeys(); j++)
                        {
                            float fTime = 
                                pNotes->keys[j]->time* SECONDSPERTICK;
                            char* pcNote = pNotes->keys[j]->note;
                                
                            unsigned int uiInsertIdx = 0;
                            for (; 
                                uiInsertIdx < uiInsertedKeys; uiInsertIdx++)
                            {
                                if (fTime <  pfTimes[uiInsertIdx])
                                {
                                    memmove(&pfTimes[uiInsertIdx+1], 
                                        &pfTimes[uiInsertIdx], 
                                        (uiInsertedKeys - uiInsertIdx)*
                                        sizeof(float));
                                    memmove(&ppcNotes[uiInsertIdx+1], 
                                        &ppcNotes[uiInsertIdx], 
                                        (uiInsertedKeys - uiInsertIdx)*
                                        sizeof(char*));
                                    break;
                                }
                            }
                            pfTimes[uiInsertIdx] = fTime;
                            ppcNotes[uiInsertIdx] = pcNote;
                            uiInsertedKeys++;
                        }
                    }
                }
            }

            
            unsigned int ui = 0;
            for (ui = 0; ui < uiTotalNoteKeys; ui++)
            {
                float fCurrentTime = pfTimes[ui];
                char* pcCurrentNote = ppcNotes[ui];
                if (NiStrnicmp(pcCurrentNote, "end", 3) == 0 && ui < 
                    uiTotalNoteKeys - 1)
                {
                    // Move end keys to the end of the keys at that time
                    unsigned int uj = ui + 1;
                    while (uj < uiTotalNoteKeys &&
                        fCurrentTime == pfTimes[uj])
                    {
                        char* pcNote = ppcNotes[uj];
                        ppcNotes[uj] = ppcNotes[uj-1];
                        ppcNotes[uj-1] = pcNote;
                        uj++;
                    }
                    ui = uj;

                }
                else if (NiStrnicmp(pcCurrentNote, "start", 5) == 0 && ui != 0)
                {
                    // Move start keys to the beginning of the keys at that 
                    // time
                    unsigned int uj = ui - 1;
                    while (uj >= 0 && fCurrentTime == pfTimes[uj])
                    {
                        char* pcNote = ppcNotes[uj];
                        ppcNotes[uj] = ppcNotes[uj+1];
                        ppcNotes[uj+1] = pcNote;
                        uj--;
                    }
                }
            }

            NiTextKey *pTextKeys;
            pTextKeys = NiNew NiTextKey[uiTotalNoteKeys];
            NIASSERT(pTextKeys);

            // Actually insert the pre-sorted keys into the array
            float fLastTime = -NI_INFINITY;
            float fLastIncTime = 0.0f;
            for (ui = 0; ui < uiTotalNoteKeys; ui++)
            {
                if (pfTimes[ui] == fLastTime)
                {
                    pfTimes[ui] += fLastIncTime + 0.00001f;
                }
                else
                {
                    fLastTime = pfTimes[ui];
                    fLastIncTime = 0.0f;
                }

                pTextKeys[ui].SetTime(pfTimes[ui]);
                pTextKeys[ui].SetText(ppcNotes[ui]);
            }

            NiFree(pfTimes);
            NiFree(ppcNotes);

            NiTextKeyExtraData *pExtra = NiNew NiTextKeyExtraData;
            NIASSERT(pExtra);
            
            pExtra->SetKeys(pTextKeys, uiTotalNoteKeys);
            return pExtra;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiMAXAnimationConverter::CanConvertPosListControl(IListControl* 
    pkControl)
{
    ListControl* pkListControl = (ListControl*) pkControl;
    int iNumKeyedAnimations = 0;
    for (int i = 0; i < pkListControl->conts.Count(); i++)
    {
        Control* pkControl = pkListControl->conts[i];
        if (pkControl != NULL)
        {
            IKeyControl* pIKeys = GetKeyControlInterface(pkControl);

            if (pIKeys != NULL)
            {
                if (pIKeys->GetNumKeys() != 0)
                {
                    iNumKeyedAnimations++;
                }
            }
            else 
            {
                Control* pXc = pkControl->GetXController();
                Control* pYc = pkControl->GetYController();
                Control* pZc = pkControl->GetZController();
                bool bFound = false;

                IKeyControl* pIKeysX = NULL;
                if (pXc)
                    pIKeysX = GetKeyControlInterface(pXc);

                if (pIKeysX != NULL && pIKeysX->GetNumKeys() != 0)
                    bFound = true;
                
                IKeyControl* pIKeysY = NULL;
                if (pYc)
                    pIKeysY = GetKeyControlInterface(pYc);

                if (pIKeysY != NULL && pIKeysY->GetNumKeys() != 0)
                    bFound = true;
                
                IKeyControl* pIKeysZ = NULL;
                if (pZc)
                    pIKeysZ = GetKeyControlInterface(pZc);

                if (pIKeysZ != NULL && pIKeysZ->GetNumKeys() != 0)
                    bFound = true;
                
                if (bFound)
                {
                    iNumKeyedAnimations++;
                }
                else if (pIKeysX == NULL && pIKeysY == NULL && 
                    pIKeysZ == NULL) 
                {
                    return false;
                }
            }
        }

    }
    return iNumKeyedAnimations == 1;
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiMAXAnimationConverter::ConvertPositionListAnim(
    IListControl*  pkControl, NiTimeController::CycleType& eCycleType, 
    INode *pMaxNode, ScaleInfo *pParentScale)
{    
    // This method assumes that out of the positionList, only one 
    // sub-controller has animation keys
    ListControl* pkListControl = (ListControl*) pkControl;
    NiInterpolatorPtr spBaseInterp = NULL;
    int iIndex = -1;
    bool bSampled = false;
    // find the animated sub-controller, save it to spBaseInterp
    // store it's index in iIndex
    for (int i = 0; i < pkListControl->conts.Count(); i++)
    {
        Control* pkControl = pkListControl->conts[i];
        bool bRetVal;   // stores whether track was sampled
        NiInterpolatorPtr spInterp = ConvertPositionAnim(pkControl, 
            eCycleType, pMaxNode, pParentScale, bRetVal);
        if (spInterp)
        {
            // at this point, we must know if the animation was sampled
            // if it was, there is no need to average poses.
            NIASSERT(spBaseInterp == NULL);
            spBaseInterp = spInterp;
            iIndex = i;
            // we only consider if the animated track was sampled
            bSampled = bRetVal;
        }
    }

    // If it's not a Point3Interpolator return NULL, which results in sampling
    if (!NiIsKindOf(NiPoint3Interpolator, spBaseInterp))
    {
        return NULL;
    }

    if (!bSampled)
    {
        NiPoint3InterpolatorPtr spPoint3Interp = 
            NiDynamicCast(NiPoint3Interpolator, spBaseInterp);

        for (unsigned int uiKeyIndex = 0; uiKeyIndex < 
            spPoint3Interp->GetKeyCount(0); uiKeyIndex++)
        {
            NiPosKey* pkKey = (NiPosKey*)spPoint3Interp->GetKeyAt(
                uiKeyIndex, 0);
            NiPoint3 kBasePt = pkKey->GetPos();

            Point3 kPoint;
            TimeValue t = 0;
            Interval valid;
            float fWeight;

            // first, multiply the original location by it's max weight
            pkListControl->pblock->GetValue(0, t, fWeight, valid, iIndex);
            fWeight = pkListControl->AverageWeight(fWeight);
            kBasePt *= fWeight;

            // next, add in the (weighted) static poses
            for(int iCtrl = 0; iCtrl < 
                pkListControl->conts.Count(); iCtrl++)
            {
                // the animated track has already been added, ignore it.
                if (iCtrl == iIndex)
                    continue;

                Control* pkControl = pkListControl->conts[iCtrl];

                pkControl->GetValue(t, &kPoint, valid);
                NiPoint3 kNewPoint(kPoint.x, kPoint.y, kPoint.z);
                pkListControl->pblock->GetValue(0, t, fWeight, valid, iCtrl);
                fWeight = pkListControl->AverageWeight(fWeight);
                // add the weighted static point to the previous position sum
                kBasePt += kNewPoint * fWeight;
            }
            pkKey->SetPos(kBasePt);
        }
    }

    return spBaseInterp;
}
//---------------------------------------------------------------------------
