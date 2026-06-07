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

#include "NiTimerSharedData.h"
#include <NiInterpController.h>
#include <NiKeyBasedInterpolator.h>

NiImplementRTTI(NiTimerSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiTimerSharedData::NiTimerSharedData() 
{ 
    m_fAccumTime = 0.0f;
    m_fLastTime = 0.0f;
    m_fScaleFactor = 1.0f;
    m_fStartTime = 0.0f;
    m_fEndTime = 0.0f;
    m_eTimeMode = LOOP;
    m_bTimingEnabled = false;
    m_fMinTime = UNINITIALIZED_FLOAT;
    m_fMaxTime = 0.0f;
    m_uiKeyframeCount = 0;
    m_uiControllerCount = 0;
}
//---------------------------------------------------------------------------
NiTimerSharedData::~NiTimerSharedData()
{ 

}
//---------------------------------------------------------------------------
float NiTimerSharedData::GetCurrentTime()
{
    return m_fAccumTime;
}
//---------------------------------------------------------------------------
float 
NiTimerSharedData::CalcCurrentTime()
{
    if(!IsEnabled())
        return m_fAccumTime;

    if(m_fLastTime == 0.0f)
        m_fLastTime = NiGetCurrentTimeInSec();

    float fCurrentTime = NiGetCurrentTimeInSec();
    float fDelta = fCurrentTime - m_fLastTime;
    fDelta *= m_fScaleFactor;

    m_fAccumTime += fDelta;

    if(m_eTimeMode == LOOP && m_fAccumTime > GetEndTime())
    {
        m_fAccumTime = GetStartTime();
    }

    m_fLastTime = fCurrentTime;
    return m_fAccumTime;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::SetCurrentTime(float fTime)
{ 
    if(fTime < 0.0f)
        return;

    if(m_fAccumTime != fTime)
    {
        if(fTime > GetEndTime() && GetTimeMode() == LOOP)
            fTime = 0.0f;
        else if(fTime < GetStartTime() && GetTimeMode() == LOOP)
            fTime = 0.0f;

        m_fAccumTime = fTime;
        m_fLastTime = 0;
    }
}
//---------------------------------------------------------------------------
float NiTimerSharedData::GetScaleFactor()
{ 
    return m_fScaleFactor;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::SetScaleFactor(float fScale)
{ 
     m_fScaleFactor = fScale;
}
//---------------------------------------------------------------------------
float NiTimerSharedData::IncrementTime(float fIncrement)
{
    m_fAccumTime += fIncrement;
    return m_fAccumTime;
}
//--------------------------------------------------------------------------
float NiTimerSharedData::GetStartTime()
{ 
    return m_fStartTime;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::SetStartTime(float fStart)
{ 
    m_fStartTime = fStart;
}
//---------------------------------------------------------------------------
float NiTimerSharedData::GetEndTime()
{ 
    return m_fEndTime;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::SetEndTime(float fEnd)
{ 
    m_fEndTime = fEnd;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::Enable()
{
    m_bTimingEnabled = true;
    m_fLastTime = 0.0f;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::Disable()
{
    m_bTimingEnabled = false;
}
//---------------------------------------------------------------------------
bool  NiTimerSharedData::IsEnabled()
{
    return m_bTimingEnabled;
}
//---------------------------------------------------------------------------
void  NiTimerSharedData::SetTimeMode(TimeMode eMode)
{
    NIASSERT(eMode >= 0 && eMode < NUM_MODES);
    m_eTimeMode = eMode;

}
//---------------------------------------------------------------------------
NiTimerSharedData::TimeMode NiTimerSharedData::GetTimeMode()
{
    return m_eTimeMode;
}
//---------------------------------------------------------------------------
void NiTimerSharedData::CollectData(NiNode* pkRoot)
{
    bool bResetStart = false;
    bool bResetEnd = false;

    if (m_fEndTime == m_fMaxTime)
        bResetEnd = true;
    if (m_fStartTime == m_fMinTime)
        bResetStart = true;

    m_fMinTime = UNINITIALIZED_FLOAT;
    m_fMaxTime = 0.0f;
    m_uiKeyframeCount = 0;
    FindAndAdd(pkRoot);
    char acString[256];
    NiOutputDebugString("NiTimerSharedData::CollectData returns:\n");
    NiSprintf(acString, 256, "\tNumber of Controllers: %d\n", 
        m_uiControllerCount);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tNumber of Keyframes: %d\n", m_uiKeyframeCount);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tMin Time: %f\n", m_fMinTime);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tMax Time: %f\n", m_fMaxTime);
    NiOutputDebugString(acString);
    //SetStartTime(GetMinKeyTime());
    //SetEndTime(GetMaxKeyTime());
}
//---------------------------------------------------------------------------
unsigned int NiTimerSharedData::GetNumberOfControllers()
{
    return m_uiControllerCount;
}
//---------------------------------------------------------------------------
unsigned int NiTimerSharedData::GetNumberOfKeyframes()
{
    return m_uiKeyframeCount;
}
//---------------------------------------------------------------------------
float NiTimerSharedData::GetMinKeyTime()
{
    if(m_fMinTime == UNINITIALIZED_FLOAT)
        return 0.0f;
    return m_fMinTime;
}
//---------------------------------------------------------------------------
float NiTimerSharedData::GetMaxKeyTime()
{
    return m_fMaxTime;
}
//---------------------------------------------------------------------------
void NiTimerSharedData::Add(NiTimeController* pkController)
{
    if(pkController == NULL)
        return;

    if(pkController->GetBeginKeyTime() < m_fMinTime)
        m_fMinTime = pkController->GetBeginKeyTime();
    if(pkController->GetEndKeyTime() > m_fMaxTime)
        m_fMaxTime = pkController->GetEndKeyTime();

    m_uiControllerCount++;

    if (NiIsKindOf(NiInterpController, pkController))
    {
        NiInterpController* pkInterpCtlr = (NiInterpController*) pkController;
        for (unsigned short us = 0; us < pkInterpCtlr->GetInterpolatorCount();
            us++)
        {
            NiInterpolator* pkInterpolator = pkInterpCtlr->GetInterpolator(
                us);
            if (pkInterpolator &&
                NiIsKindOf(NiKeyBasedInterpolator, pkInterpolator))
            {
                AddInfo((NiKeyBasedInterpolator*) pkInterpolator);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiTimerSharedData::FindAndAdd(NiObjectNET* pkRoot)
{
   if(pkRoot == NULL)
        return;

   
   NiTimeController* pkController = pkRoot->GetControllers();

   // Gather all time controllers from this object
   while(pkController != NULL)
   {
       Add(pkController);
       pkController = pkController->GetNext();
   }

   if(NiIsKindOf(NiAVObject, pkRoot))
   {
       // NiProperties can have time controllers, so search them too
       NiAVObject* pkObj = (NiAVObject*) pkRoot;
       NiTListIterator kPos = pkObj->GetPropertyList().GetHeadPos();
       
       while(kPos != NULL)
       {
           NiProperty* pkProperty = pkObj->GetPropertyList().GetNext(kPos);
           if(pkProperty)
               FindAndAdd(pkProperty);
       }
   }
   if(NiIsKindOf(NiNode, pkRoot))
   {
        NiNode* pkNode = (NiNode*) pkRoot;
        // Search all of the children
        for(unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkObj = pkNode->GetAt(ui);
            FindAndAdd(pkObj);
        }
        // NiDynamicEffects can have time controllers, so search them too
        NiTListIterator kPos = pkNode->GetEffectList().GetHeadPos();
        while(kPos)
        {
           NiDynamicEffect* pkEffect = pkNode->GetEffectList().GetNext(kPos);
           if(pkEffect)
               FindAndAdd(pkEffect);
        }
        
   }
}
//---------------------------------------------------------------------------
void NiTimerSharedData::AddInfo(NiKeyBasedInterpolator* pkInterp)
{
    for (unsigned short usChannel = 0;
        usChannel < pkInterp->GetKeyChannelCount(); usChannel++)
    {
        m_uiKeyframeCount += pkInterp->GetKeyCount(usChannel);
    }
}
//---------------------------------------------------------------------------
