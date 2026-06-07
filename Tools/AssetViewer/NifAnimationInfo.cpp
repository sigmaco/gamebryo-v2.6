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

// NifAnimationInfo.cpp

#include "stdafx.h"
#include "NifAnimationInfo.h"

//---------------------------------------------------------------------------
CNifAnimationInfo::CNifAnimationInfo()
{
    m_fMinTime = UNINITIALIZED_FLOAT;
    m_fMaxTime = 0.0f;
    m_uiKeyframeCount = 0;
    m_uiControllerCount = 0;
}
//---------------------------------------------------------------------------
CNifAnimationInfo::~CNifAnimationInfo()
{

}
//---------------------------------------------------------------------------
void CNifAnimationInfo::CollectData(NiNode* pkRoot)
{
    m_fMinTime = UNINITIALIZED_FLOAT;
    m_fMaxTime = 0.0f;
    m_uiKeyframeCount = 0;
    FindAndAdd(pkRoot);
    char acString[256];
    NiOutputDebugString("CNifAnimationInfo::CollectData returns:\n");
    NiSprintf(acString, 256, "\tNumber of Controllers: %d\n",
        m_uiControllerCount);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tNumber of Keyframes: %d\n",
        m_uiKeyframeCount);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tMin Time: %f\n", m_fMinTime);
    NiOutputDebugString(acString);
    NiSprintf(acString, 256, "\tMax Time: %f\n", m_fMaxTime);
    NiOutputDebugString(acString);
}
//---------------------------------------------------------------------------
unsigned int CNifAnimationInfo::GetNumberOfControllers()
{
    return m_uiControllerCount;
}
//---------------------------------------------------------------------------
unsigned int CNifAnimationInfo::GetNumberOfKeyframes()
{
    return m_uiKeyframeCount;
}
//---------------------------------------------------------------------------
float CNifAnimationInfo::GetMinTime()
{
    if(m_fMinTime == UNINITIALIZED_FLOAT)
        return 0.0f;
    return m_fMinTime;
}
//---------------------------------------------------------------------------
float CNifAnimationInfo::GetMaxTime()
{
    return m_fMaxTime;
}
//---------------------------------------------------------------------------
void CNifAnimationInfo::Add(NiTimeController* pkController)
{
    if(pkController == NULL)
        return;

    AddInfo(pkController);

    if(pkController->GetBeginKeyTime() < m_fMinTime)
        m_fMinTime = pkController->GetBeginKeyTime();
    if(pkController->GetEndKeyTime() > m_fMaxTime)
        m_fMaxTime = pkController->GetEndKeyTime();

    m_uiControllerCount++;

    if(NiIsKindOf(NiInterpController, pkController))
        AddInterpInfo((NiInterpController*) pkController);
}
//---------------------------------------------------------------------------
void CNifAnimationInfo::FindAndAdd(NiObjectNET* pkRoot)
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
       NiPropertyList* pkPropList = &(pkObj->GetPropertyList());
       NiTListIterator kIter = pkPropList->GetHeadPos();
       while(pkPropList != NULL && !pkPropList->IsEmpty()&& kIter)
       {
           NiProperty* pkProperty = pkPropList->GetNext(kIter);
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
        const NiDynamicEffectList* pkEffectList= &(pkNode->GetEffectList());

        NiTListIterator kIter = pkEffectList->GetHeadPos();
        while(pkEffectList != NULL && !pkEffectList->IsEmpty() && kIter)
        {
           NiDynamicEffect* pkEffect = pkEffectList->GetNext(kIter);
           if(pkEffect)
               FindAndAdd(pkEffect);
        }
        
   }
}
//---------------------------------------------------------------------------
void CNifAnimationInfo::AddInfo(NiTimeController* pkController)
{
}
//---------------------------------------------------------------------------
void CNifAnimationInfo::AddInterpInfo(NiInterpController* pkController)
{
    for (unsigned int us = 0; us <  pkController->
        GetInterpolatorCount(); us++)
    {
        NiInterpolator* pkInterp = pkController->GetInterpolator(us);
        if (NiIsKindOf(NiKeyBasedInterpolator, pkInterp))
        {
            NiKeyBasedInterpolator* pkKeyInterp =
                (NiKeyBasedInterpolator*) pkInterp;
            for (unsigned short usc = 0; usc < pkKeyInterp->
                GetKeyChannelCount(); usc++)
            {
                m_uiKeyframeCount += pkKeyInterp->GetKeyCount(usc);
            }
        }
    }
}
//---------------------------------------------------------------------------
