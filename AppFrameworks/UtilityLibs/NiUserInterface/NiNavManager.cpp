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
#include "NiUserInterfacePCH.h"

#include "NiNavManager.h"
#include "NiUIManager.h"
//---------------------------------------------------------------------------
NiNavManager* NiNavManager::m_pkNavManager = NULL;
//---------------------------------------------------------------------------
NiNavManager::NiNavManager() :
    m_kActiveControllerChangedSignal(),
    m_fLastFrameTime(0.f),
    m_fThisFrameDelta(0.f),
    m_uiCurController(0)
{
}
//---------------------------------------------------------------------------
NiNavManager::~NiNavManager()
{
}
//---------------------------------------------------------------------------
bool NiNavManager::Create()
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    if (m_pkNavManager != NULL)
        return true;

    m_pkNavManager = NiNew NiNavManager();
    return true;
}
//---------------------------------------------------------------------------
void NiNavManager::SubscribeToControllerChanged(
    NiUIBaseSlot1<NiNavBaseController*>* kControllerChangedSlot)
{
    m_kActiveControllerChangedSignal.Subscribe(kControllerChangedSlot);
}
//---------------------------------------------------------------------------
NiNavBaseController* NiNavManager::GetNavControllerAt(unsigned int ui) const
{
    if (ui < m_kControllers.GetSize())
        return m_kControllers.GetAt(ui);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiNavBaseController* NiNavManager::GetCurrentNavController() const
{
    return GetNavControllerAt(m_uiCurController);
}
//---------------------------------------------------------------------------
unsigned int NiNavManager::GetCurrentNavControllerIndex() const
{
    return m_uiCurController;
}
//---------------------------------------------------------------------------
unsigned int NiNavManager::AddNavigationController(NiNavBaseController* 
    pkNavController)
{
    if (pkNavController == NULL)
        return (unsigned int)-1;
    // We only want controllers in there once
    if (GetControllerIdx(pkNavController) != -1)
        return (unsigned int)-1;

    unsigned int uiCurSize = m_kControllers.GetSize();
    NiNavBaseController::NavState kNavState;
    if (uiCurSize == 0)
    {
        m_uiCurController = 0;
        kNavState.bIgnoreThis = true;
        pkNavController->SetActive(true, kNavState);
        m_kActiveControllerChangedSignal.EmitSignal(pkNavController);
    }
    else 
    {
        pkNavController->SetActive(false, kNavState);
    }

    return m_kControllers.Add(pkNavController);
}
//---------------------------------------------------------------------------
void NiNavManager::RemoveNavigationController(
    NiNavBaseController* pkNavController)
{
    int iIdx = GetControllerIdx(pkNavController);
    if (iIdx == -1)
        return;
    if (pkNavController == NULL)
        return;

    NiNavBaseController::NavState kNavState;
    kNavState.bIgnoreThis = true;
    unsigned int uiCurSize = m_kControllers.GetSize();
    bool bWasActive = pkNavController->IsActive();
    if (uiCurSize == 1)
    {
        m_uiCurController = 0;
        
        if (bWasActive)
            pkNavController->SetActive(false, kNavState);
        m_kControllers.RemoveAt((unsigned int)iIdx);
        m_kControllers.SetSize(0);
    }
    else
    {
        m_kControllers.RemoveAtAndFill((unsigned int)iIdx);
        m_uiCurController = 0;
        if (bWasActive)
            pkNavController->SetActive(false, kNavState);
    }
   
    if (bWasActive)
        AdvanceController();
}
//---------------------------------------------------------------------------
void NiNavManager::AdvanceController()
{
    unsigned int uiSize = m_kControllers.GetSize();
    if (uiSize == 0)
        return;
    NiNavBaseController::NavState kNavState;
    kNavState.bIgnoreThis = false;
    NiNavBaseController* pkCurController;
    pkCurController = m_kControllers.GetAt(m_uiCurController);
    pkCurController->SetActive(false, kNavState);

    ++m_uiCurController;
    m_uiCurController = m_uiCurController % uiSize;
    pkCurController = m_kControllers.GetAt(m_uiCurController);
    pkCurController->SetActive(true, kNavState);
    m_kActiveControllerChangedSignal.EmitSignal(pkCurController);

}
//---------------------------------------------------------------------------
void NiNavManager::SetActiveController(NiNavBaseController* pkNavController)
{
    unsigned int uiSize = m_kControllers.GetSize();
    if (uiSize == 0 || pkNavController == NULL)
        return;

    unsigned int uiIdx = UINT_MAX;
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        if (m_kControllers.GetAt(ui) == pkNavController)
        {
            uiIdx = ui;
            break;
        }
    }

    if (uiIdx == UINT_MAX)
        return;

    NiNavBaseController::NavState kNavState;
    kNavState.bIgnoreThis = false;
    NiNavBaseController* pkCurController;
    pkCurController = m_kControllers.GetAt(m_uiCurController);
    pkCurController->SetActive(false, kNavState);

    m_uiCurController = uiIdx;
    pkCurController = m_kControllers.GetAt(m_uiCurController);
    pkCurController->SetActive(true, kNavState);
    m_kActiveControllerChangedSignal.EmitSignal(pkCurController);

}
//---------------------------------------------------------------------------
