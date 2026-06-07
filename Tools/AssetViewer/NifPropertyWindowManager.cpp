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

// NifPropertyWindowManager.cpp

#include "stdafx.h"
#include "NifPropertyWindowManager.h"
#include "NifPropertyWindow.h"
#include "NifPropertyWindowFactory.h"
#include "NiObjectPropertyWindowCreator.h"
#include "NiObjectNETPropertyWindowCreator.h"
#include "NiAVObjectPropertyWindowCreator.h"
#include "NiKeyBasedInterpolatorPropertyWindowCreator.h"
#include "NiNodePropertyWindowCreator.h"
#include "NiTimeControllerPropertyWindowCreator.h"
#include "NiInterpControllerPropertyWindowCreator.h"
#include "NiGeometryPropertyWindowCreator.h"
#include "NiGeometryDataPropertyWindowCreator.h" 
#include "NiMeshPropertyWindowCreator.h"
#include "NiMeshModifierPropertyWindowCreator.h"
#include "NiDataStreamPropertyWindowCreator.h"
#include "NiParticleSystemPropertyWindowCreator.h"
#include "NiCameraPropertyWindowCreator.h"
#include "NiDynamicEffectPropertyWindowCreator.h"
#include "NiLightPropertyWindowCreator.h"
#include "NiMaterialPropertyPropertyWindowCreator.h"
#include "NiTexturingPropertyPropertyWindowCreator.h"
#include "NiVertexColorPropPropertyWindowCreator.h"
#include "NiShadowGeneratorPropertyWindowCreator.h"
#include "NifDoc.h"

CNifPropertyWindowManager* CNifPropertyWindowManager::ms_pkThis = NULL;

//---------------------------------------------------------------------------
CNifPropertyWindowManager::CNifPropertyWindowManager()
{
    m_kWindowsArray.SetSize(25);
    m_kWindowsArray.SetGrowBy(25);
    m_fLastTimeUpdated = -1.0f;

    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiObjectPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiObjectNETPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiAVObjectPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiKeyBasedInterpolatorPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiNodePropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiTimeControllerPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiInterpControllerPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiGeometryPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiMeshPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiMeshModifierPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiDataStreamPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiParticleSystemPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiGeometryDataPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiCameraPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiDynamicEffectPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiLightPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiMaterialPropertyPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiTexturingPropertyPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiVertexColorPropertyPropertyWindowCreator());
    CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
        NiNew CNiShadowGeneratorPropertyWindowCreator());
}
//---------------------------------------------------------------------------
CNifPropertyWindowManager::~CNifPropertyWindowManager()
{
    KillAllPropertyWindows();
    CNifPropertyWindowFactory::RemoveAllCreators();
}
//---------------------------------------------------------------------------
CNifPropertyWindowManager* CNifPropertyWindowManager::
    GetPropertyWindowManager()
{
    if(ms_pkThis == NULL)
        ms_pkThis = NiNew CNifPropertyWindowManager();
    return ms_pkThis;
}
//---------------------------------------------------------------------------
CNifPropertyWindow* CNifPropertyWindowManager::CreatePropertyWindow(
    NiObject* pkObj)
{
    static int iWidth = 450;
    static int iHeight = 640;
    static int iStartX = 100;
    static int iStartY = 100;
    CNifPropertyWindow* pkWindow = NULL;

    pkWindow = GetPropertyWindowForNiObject(pkObj);

    if(pkWindow)
    {
        pkWindow->SetActiveWindow();
    }
    else
    {
        pkWindow = CNifPropertyWindowFactory::CreatePropertyWindow(pkObj,
            iStartX, iStartY, iWidth, iHeight);

        if(pkWindow)
        {
            pkWindow->SetNiObject(pkObj);
            m_kWindowsArray.Add(pkWindow);
            iStartX += 20;
            iStartY += 20;

            if (iStartX > iWidth)
                iStartX = 100;
            if (iStartY > iHeight)
                iStartY = 100;
        }
    }

    return pkWindow;
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::KillAllPropertyWindows()
{
    for(unsigned int ui = 0; ui < m_kWindowsArray.GetSize(); ui++)
    {
        delete m_kWindowsArray.RemoveAt(ui);
    }
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::RemovePropertyWindow(
    CNifPropertyWindow* pkWindow)
{
    for(unsigned int ui = 0; ui < m_kWindowsArray.GetSize(); ui++)
    {
        if(m_kWindowsArray.GetAt(ui) == pkWindow)
        {
            delete m_kWindowsArray.RemoveAt(ui);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::DestroyPropertyWindowManager()
{
    delete ms_pkThis;
    ms_pkThis = NULL;
}

//---------------------------------------------------------------------------
void CNifPropertyWindowManager::UpdateAllVisiblePropertyWindows(
    bool bForceUpdate)
{
    CNifPropertyWindow* pkWindow;
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc)
        return;

    pkDoc->Lock();

    if(pkDoc->GetLastTimeUpdated() != m_fLastTimeUpdated || 
       bForceUpdate)
    {
        m_fLastTimeUpdated = pkDoc->GetLastTimeUpdated();
    }
    else
    {
        pkDoc->UnLock();
        return;
    }

    for(unsigned int ui = 0; ui < m_kWindowsArray.GetSize(); ui++)
    {
        pkWindow = m_kWindowsArray.GetAt(ui);

        if(pkWindow && pkWindow->HasHeartbeat() &&
           pkWindow->m_hWnd != NULL &&
           pkWindow->IsWindowVisible() &&
           pkWindow->IsWindowEnabled() && !pkWindow->IsIconic())
        {
            pkWindow->UpdateActivePages();   
        }
        else if(pkWindow && !pkWindow->HasHeartbeat())
        {
            RemovePropertyWindow(pkWindow);
        }
    }

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
CNifPropertyWindow*  CNifPropertyWindowManager::GetPropertyWindowForNiObject(
    NiObject* pkObj)
{
    CNifPropertyWindow* pkWindow;
    CNifDoc* pkDoc = CNifDoc::GetDocument();

    if(!pkDoc || !pkObj)
        return NULL;

    pkDoc->Lock();

    for(unsigned int ui = 0; ui < m_kWindowsArray.GetSize(); ui++)
    {
        pkWindow = m_kWindowsArray.GetAt(ui);
        if(pkWindow && pkWindow->HasHeartbeat() &&
           pkWindow->m_hWnd != NULL &&
           pkWindow->GetNiObject() == pkObj)
        {
            pkDoc->UnLock();
            return pkWindow;
        }
    }

    pkDoc->UnLock();
    return NULL;
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::OnRemoveRoot()
{
    CNifPropertyWindow* pkWindow;
    CNifDoc* pkDoc = CNifDoc::GetDocument();

    if(!pkDoc)
        return;

    pkDoc->Lock();

    for(unsigned int ui = 0; ui < m_kWindowsArray.GetSize(); ui++)
    {
        pkWindow = m_kWindowsArray.GetAt(ui);
        if(pkWindow && pkWindow->HasHeartbeat())
        {
            if(pkWindow->PerformSuicideWatch())
                RemovePropertyWindow(pkWindow);
        }
        else if(pkWindow)
            RemovePropertyWindow(pkWindow);
    }

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::OnDestroyScene()
{
    OnRemoveRoot();
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::OnUpdate(CView* pSender, LPARAM lHint,
    CObject* pHint) 
{
    WORD wLoWord = LOWORD(lHint);
    switch (wLoWord)
    {
        case NIF_DESTROYSCENE:
            OnDestroyScene();
            break;
        case NIF_CREATESCENE:
            break;
        case NIF_ADDNIF:
            break;
        case NIF_REMOVENIF:
            OnRemoveRoot();
            break;
        case NIF_UPDATEDEFAULTNODE:
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void CNifPropertyWindowManager::SendUpdateMessage(CView* pSender,
    LPARAM lHint, CObject* pHint)
{
    if(ms_pkThis)
        ms_pkThis->OnUpdate(pSender, lHint, pHint);
}
//---------------------------------------------------------------------------
