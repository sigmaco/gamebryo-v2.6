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
#include "NiMainPCH.h"
#include "NiTimeController.h"

#include "NiCloningProcess.h"
#include "NiCriticalSection.h"
#include "NiRenderObject.h"
#include "NiStream.h"
#include "NiNode.h"
#include "NiGeometry.h"
#include <NiSystem.h>

NiImplementRTTI(NiTimeController,NiObject);

NiFastCriticalSection NiTimeController::ms_kCachedValuesLock;

//---------------------------------------------------------------------------
NiTimeController::NiTimeController() :
    m_uFlags(0),
    m_fFrequency(1.0f),
    m_fPhase(0.0f),
    m_fLoKeyTime(NI_INFINITY),
    m_fHiKeyTime(-NI_INFINITY),
    m_fStartTime(-NI_INFINITY),
    m_fLastTime(-NI_INFINITY),
    m_fWeightedLastTime(0.0f),
    m_fScaledTime(-NI_INFINITY),
    m_pkTarget(NULL)
{
    SetAnimType(APP_TIME);
    SetCycleType(CLAMP);
    SetActive(true);
    SetPlayBackwards(false);
    SetComputeScaledTime(true);
    SetForceUpdate(false);
}
//---------------------------------------------------------------------------
NiTimeController::~NiTimeController()
{
    /* */
}
//---------------------------------------------------------------------------
unsigned int NiTimeController::ItemsInList() const
{
    const NiTimeController* pkControl = this;
    unsigned int uiCount = 0;

    for (/**/; pkControl; pkControl = pkControl->m_spNext)
        uiCount++;

    return uiCount;
}
//---------------------------------------------------------------------------
void NiTimeController::Start(float fTime) 
{
    SetActive(true);
    m_fLastTime = -NI_INFINITY;
    if (GetAnimType() == APP_INIT)
    {
        m_fStartTime = fTime;
    }
}
//---------------------------------------------------------------------------
void NiTimeController::Stop() 
{
    SetActive(false);
    m_fLastTime = -NI_INFINITY;
    if (GetAnimType() == APP_INIT)
    {
        m_fStartTime = -NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
// ComputeScaledTime converts the incoming time value
// to scaled time using the object's frequency and phase
// values in combination with the cycle type.  The 
// returned scaled time will most often represent the
// time units found in the keyframes.  
float NiTimeController::ComputeScaledTime(float fTime)
{
    static CycleType seLastCycle = MAX_CYCLE_TYPES;
    static float sfLastHi;
    static float sfLastLo;
    static float sfLastTime;
    static float sfLastScaled;

    float fScaledTime = 0.0f;

    // If this is the first time Update is being called then we start
    // the animation at m_fPhase.  If the animation is APP_INIT, reset
    // the delta time to 0.
    if (m_fStartTime == -NI_INFINITY)
    {
        m_fStartTime = fTime;
    }

    float fDeltaTime;
    if (m_fLastTime == -NI_INFINITY)
    {
        m_fWeightedLastTime = 0.0f;

        if (GetAnimType() == APP_INIT)
        {
            fDeltaTime = 0.0f;
        }
        else
        {
            fDeltaTime = fTime;
        }
    }
    else
    {
        // Calculate time since last update
        fDeltaTime = fTime - m_fLastTime;
    }


    // Scale time linearly by frequency and phase values 
    // specified by the application.
    //
    // time' = freq * time + phase
    //
    m_fWeightedLastTime += fDeltaTime * m_fFrequency;
    fScaledTime = m_fWeightedLastTime + m_fPhase;

    m_fLastTime = fTime;
    CycleType eCycle = GetCycleType();

    ms_kCachedValuesLock.Lock();
    if (sfLastHi == m_fHiKeyTime && sfLastLo == m_fLoKeyTime && 
        sfLastTime == fScaledTime && seLastCycle == eCycle)
    {
        float fReturn = sfLastScaled;
        ms_kCachedValuesLock.Unlock();
        return fReturn;
    }

    sfLastHi = m_fHiKeyTime;
    sfLastLo = m_fLoKeyTime;
    sfLastTime = fScaledTime;
    seLastCycle = eCycle;

    if (m_fHiKeyTime == -NI_INFINITY || m_fLoKeyTime == NI_INFINITY)
    {
        // Avoiding overflow from span computation -
        // do nothing, conditions below will handle clamping.
    }
    else switch (eCycle) // Switch on m_CycleType
    {
        case LOOP:
        {
            //
            // Loop -- only need fractional time
            //
            float fSpan = m_fHiKeyTime - m_fLoKeyTime;

            if (fSpan != 0.0f)
            {
                fScaledTime = NiFmod(fScaledTime - m_fLoKeyTime, fSpan) +
                    m_fLoKeyTime;

                if (fScaledTime < m_fLoKeyTime)
                {
                    fScaledTime += fSpan;
                }
            }
            else
            {
                fScaledTime = m_fLoKeyTime;
            }

            break;
        }
        case REVERSE:
        {
            //
            // Reverse -- If the integer portion of the scaled 
            // time divided by the delta time is odd, then the 
            // animation is on a reverse leg of the sequence.
            //
            // assert: m_fHiKeyTime != m_fLoKeyTime
            //
            float fSpan = m_fHiKeyTime - m_fLoKeyTime;
            float fTimeFract;

            if (fSpan != 0.0f)
            {
                float fDoubleSpan = 2.0f * fSpan;
                fTimeFract = NiFmod(fScaledTime, fDoubleSpan);
                if (fTimeFract < 0.0f)
                {
                    fTimeFract += fDoubleSpan;
                }

                if (fTimeFract > fSpan)
                    fScaledTime = fDoubleSpan - fTimeFract + m_fLoKeyTime;
                else
                    fScaledTime = fTimeFract + m_fLoKeyTime;
            }
            else
                fScaledTime = m_fLoKeyTime;
            
            break;
        }
        case CLAMP:
        {
            // Clamp - do nothing, conditions below will handle clamping.
            break;
        }
        default:
            // perhaps some better error protection, like an NIASSERT() here?
            break;
    }

    if (fScaledTime > m_fHiKeyTime)
        fScaledTime = m_fHiKeyTime;
    else if (fScaledTime < m_fLoKeyTime)
        fScaledTime = m_fLoKeyTime;

    if (GetPlayBackwards())
    {
        fScaledTime = m_fHiKeyTime - (fScaledTime - m_fLoKeyTime);
    }

    sfLastScaled = fScaledTime;
    ms_kCachedValuesLock.Unlock();

    return fScaledTime;
}
//---------------------------------------------------------------------------
void NiTimeController::StartAnimations(NiObjectNET* pkObj)
{
    // Recurse down the scene graph calling start on all NiTimeController
    // derived classes.

    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
        pkControl->Start();             
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;

        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pkProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pkProperty && pkProperty->GetControllers())
                StartAnimations(pkProperty);
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;

        // recurse on children
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;

            pkChild = pkNode->GetAt(i);
            if (pkChild)
                StartAnimations(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void NiTimeController::StartAnimations(NiObjectNET* pkObj, float fTime)
{
    // Recurse down the scene graph calling start on all NiTimeController
    // derived classes with the specified time.

    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
        pkControl->Start(fTime);             
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;

        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pProperty && pProperty->GetControllers())
                StartAnimations(pProperty, fTime);
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode;
        unsigned int i;
            
        pkNode = (NiNode*) pkObj;

        // recurse on children
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;

            pkChild = pkNode->GetAt(i);
            if (pkChild)
                StartAnimations(pkChild, fTime);
        }
    }
}
//---------------------------------------------------------------------------
void NiTimeController::StopAnimations(NiObjectNET* pkObj)
{
    // Recurse down the scene graph calling stop on all NiTimeController
    // derived classes.

    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
        pkControl->Stop();             
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;

        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pProperty && pProperty->GetControllers())
                StopAnimations(pProperty);
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode;
        unsigned int i;
            
        pkNode = (NiNode*) pkObj;

        // recurse on children
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;

            pkChild = pkNode->GetAt(i);
            if (pkChild)
                StopAnimations(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void NiTimeController::SetTarget(NiObjectNET* pkTarget)
{
    if (m_pkTarget == pkTarget)
        return;

    // Increment ref count to insure that "this" is not deleted when it is 
    // detached from m_pkTarget.
    IncRefCount();

    // remove from old list
    if (m_pkTarget)
    {
        if (m_pkTarget->GetControllers())
            m_pkTarget->RemoveController(this);
    }

    m_pkTarget = pkTarget;

    // add to new list
    if (m_pkTarget)
    {
        NIASSERT(TargetIsRequiredType());

        // add controller to list only if it is not already in list
        NiTimeController* pkControl = m_pkTarget->GetControllers();
        while (pkControl)
        {
            if (pkControl == this)
            {
                DecRefCount();
                return;
            }
            pkControl = pkControl->GetNext();
        }

        m_pkTarget->PrependController(this);
    }

    DecRefCount();
}
//---------------------------------------------------------------------------
void NiTimeController::OnPreDisplay()
{
}
//---------------------------------------------------------------------------
void NiTimeController::GetMinBeginKeyTimeRecursive(NiAVObject* pkObject,
    float& fMinBeginKeyTime)
{
    NIASSERT(pkObject);

    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        if (pkCtlr->GetBeginKeyTime() < fMinBeginKeyTime)
        {
            fMinBeginKeyTime = pkCtlr->GetBeginKeyTime();
        }

        pkCtlr = pkCtlr->GetNext();
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ++ui)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                GetMinBeginKeyTimeRecursive(pkChild, fMinBeginKeyTime);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiTimeController::ResetModifierData()
{
    // Do nothing in default implementation
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiTimeController::CopyMembers(NiTimeController* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    pkDest->m_fFrequency = m_fFrequency;
    pkDest->m_fPhase = m_fPhase;
    pkDest->m_fLoKeyTime = m_fLoKeyTime;
    pkDest->m_fHiKeyTime = m_fHiKeyTime;
    pkDest->m_fStartTime = m_fStartTime;
    pkDest->m_fLastTime = m_fLastTime;
    pkDest->m_fWeightedLastTime = m_fWeightedLastTime;

    if (m_pkTarget)
    {
        NiObject* pkClone;
        bool bCloned = kCloning.m_pkCloneMap->GetAt(m_pkTarget, pkClone);
        if (bCloned)
            pkDest->m_pkTarget = (NiObjectNET*)pkClone;
        else
            pkDest->m_pkTarget = NULL;
    }

    if (m_spNext != NULL)
    {
        pkDest->m_spNext = (NiTimeController*)m_spNext->CreateClone(
            kCloning);
    }
}
//---------------------------------------------------------------------------
void NiTimeController::ProcessClone(
    NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    if (m_spNext)
    {
        m_spNext->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiTimeController::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.ReadLinkID();    // m_spNext

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadBinary(kStream, m_fFrequency);
    NiStreamLoadBinary(kStream, m_fPhase);
    NiStreamLoadBinary(kStream, m_fLoKeyTime);
    NiStreamLoadBinary(kStream, m_fHiKeyTime);
    kStream.ReadLinkID();    // m_spTarget

    // Variables should not be streamed in; return to initialized values.
    SetComputeScaledTime(true);
    SetForceUpdate(false);
}
//---------------------------------------------------------------------------
void NiTimeController::LinkObject(NiStream& kStream)
{
    m_spNext = (NiTimeController*) kStream.GetObjectFromLinkID();
    
    m_pkTarget = (NiObjectNET*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiTimeController::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    if (m_spNext)
        m_spNext->RegisterStreamables(kStream);

    // The m_pkTarget member must not register itself since that object has
    // already made the call earlier (the reason 'this' is registering
    // itself).

    return true;
}
//---------------------------------------------------------------------------
void NiTimeController::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    // Ensure we save LinkID only for streamable NiTimeControllers
    NiTimeController* pkController = m_spNext;
    while (pkController && !pkController->IsStreamable())
        pkController = pkController->GetNext();
    kStream.SaveLinkID(pkController);
    
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_fFrequency);
    NiStreamSaveBinary(kStream, m_fPhase);
    NiStreamSaveBinary(kStream, m_fLoKeyTime);
    NiStreamSaveBinary(kStream, m_fHiKeyTime);
    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
bool NiTimeController::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    // assert:  pVoid is NiTimeController-derived
    NiTimeController* pkControl = (NiTimeController*) pkObject;

    if (m_uFlags != pkControl->m_uFlags ||
        m_fFrequency != pkControl->m_fFrequency ||
        m_fPhase != pkControl->m_fPhase ||
        m_fLoKeyTime != pkControl->m_fLoKeyTime  ||
        m_fHiKeyTime != pkControl->m_fHiKeyTime)
    {
        return false;
    }

    if ((m_pkTarget && !pkControl->m_pkTarget) ||
       (!m_pkTarget && pkControl->m_pkTarget))
    {
        return false;
    }

    // do not call IsEqual on m_pkTarget. Since this routine was called
    // from m_pkTarget's IsEqual routine we'll just cause an infinite
    // recursive loop.

    return true;
}
//---------------------------------------------------------------------------
void NiTimeController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiTimeController::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_fFrequency", m_fFrequency));
    pkStrings->Add(NiGetViewerString("m_fPhase", m_fPhase));
    pkStrings->Add(NiGetViewerString("m_fLoKeyTime", m_fLoKeyTime));
    pkStrings->Add(NiGetViewerString("m_fHiKeyTime", m_fHiKeyTime));
    pkStrings->Add(NiGetViewerString("m_fStartTime", m_fStartTime));
    pkStrings->Add(NiGetViewerString("m_fLastTime", m_fLastTime));
    pkStrings->Add(NiGetViewerString("m_pkTarget", m_pkTarget));
    pkStrings->Add(GetViewerString("anim type", GetAnimType()));
    pkStrings->Add(GetViewerString("cycle type", GetCycleType()));
    pkStrings->Add(NiGetViewerString("Active", GetActive())); 
    pkStrings->Add(NiGetViewerString("Play Backwards", GetPlayBackwards()));
}
//---------------------------------------------------------------------------
char* NiTimeController::GetViewerString(const char* pcPrefix, AnimType eType)
{
    size_t stLen = strlen(pcPrefix) + 12;
    char* pcString = NiAlloc(char, stLen);

    switch(eType)
    {
    case APP_TIME:
        NiSprintf(pcString, stLen, "%s = APP_TIME", pcPrefix);
        break;
    case APP_INIT:
        NiSprintf(pcString, stLen, "%s = APP_INIT", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------
char* NiTimeController::GetViewerString(const char* pcPrefix, CycleType eType)
{
    size_t stLen = strlen(pcPrefix) + 11;
    char* pcString = NiAlloc(char, stLen);

    switch(eType)
    {
    case LOOP:
        NiSprintf(pcString, stLen, "%s = LOOP", pcPrefix);
        break;
    case REVERSE:
        NiSprintf(pcString, stLen, "%s = REVERSE", pcPrefix);
        break;
    case CLAMP:
        NiSprintf(pcString, stLen, "%s = CLAMP", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;    
    }

    return pcString;
}
//---------------------------------------------------------------------------
