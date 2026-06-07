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

// NiParticleSystemDlg.cpp

#include "stdafx.h"
#include "commctrl.h"
#include "AssetViewer.h"
#include "NiParticleSystemDlg.h"
#include "NifPropertyWindowManager.h"

#include <NiPSParticleSystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiParticleSystemDlg dialog
//---------------------------------------------------------------------------
CNiParticleSystemDlg::CNiParticleSystemDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiParticleSystemDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiParticleSystemDlg)
    //}}AFX_DATA_INIT
}
CNiParticleSystemDlg::~CNiParticleSystemDlg()
{
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiParticleSystemDlg)
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_MAXCOUNT_EDIT, 
        m_wndParticleCountMax);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_COUNT_EDIT, 
        m_wndParticleCountCurrent);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_WORLDSPACE_EDIT, 
        m_wndIsWorldSpace);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_COLORS_EDIT, 
        m_wndHasColors);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_ROTATION_EDIT, 
        m_wndHasRotation);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_ROTATION_AXIS_EDIT, 
        m_wndHasRotationAxis);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_EMITTERS_LIST, m_wndEmittersList);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_SPAWNERS_LIST, m_wndSpawnersList);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_DEATH_SPAWNER_BUTTON, 
        m_wndDeathSpawnerButton);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_FORCES_LIST, m_wndForcesList);
    DDX_Control(pDX, IDC_NIPARTICLESYSTEM_COLLIDERS_LIST, m_wndCollidersList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiParticleSystemDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiParticleSystemDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIPARTICLESYSTEM_EMITTERS_LIST, 
        OnDblclkEmittersList)
    ON_NOTIFY(NM_DBLCLK, IDC_NIPARTICLESYSTEM_SPAWNERS_LIST, 
        OnDblclkSpawnersList)
    ON_BN_CLICKED(IDC_NIPARTICLESYSTEM_DEATH_SPAWNER_BUTTON, 
        OnDeathSpawnerButton)
    ON_NOTIFY(NM_DBLCLK, IDC_NIPARTICLESYSTEM_FORCES_LIST, 
        OnDblclkForcesList)
    ON_NOTIFY(NM_DBLCLK, IDC_NIPARTICLESYSTEM_COLLIDERS_LIST, 
        OnDblclkCollidersList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiParticleSystemDlg message handlers
//---------------------------------------------------------------------------
bool CNiParticleSystemDlg::DoUpdate()
{
    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return false;

    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;
    
    // String buffer
    char acString[256];

    NiSprintf(acString, 256, "%d", pkParticleSystem->GetMaxNumParticles());
    m_wndParticleCountMax.SetWindowText(acString);

    NiSprintf(acString, 256, "%d", pkParticleSystem->GetNumParticles());
    m_wndParticleCountCurrent.SetWindowText(acString);

    NiSprintf(acString, 256, "%s", 
        pkParticleSystem->GetWorldSpace() ? "true" : "false");
    m_wndIsWorldSpace.SetWindowText(acString);

    NiSprintf(acString, 256, "%s", 
        pkParticleSystem->HasColors() ? "true" : "false");
    m_wndHasColors.SetWindowText(acString);

    NiSprintf(acString, 256, "%s", 
        pkParticleSystem->HasRotations() ? "true" : "false");
    m_wndHasRotation.SetWindowText(acString);

    NiSprintf(acString, 256, "%s", 
        pkParticleSystem->HasRotationAxes() ? "true" : "false");
    m_wndHasRotationAxis.SetWindowText(acString);

    int nItem = 0;
    NiUInt32 uiCount = 0;

    // Emitters list
    m_wndEmittersList.DeleteAllItems();
    uiCount = pkParticleSystem->GetEmitterCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSEmitter* pkEmitter = pkParticleSystem->GetEmitterAt(ui);
        if (pkEmitter)
        {
            NiSprintf(acString, 256, "%d", nItem);
            m_wndEmittersList.InsertItem(nItem, acString);

            NiSprintf(acString, 256, "%s (%s)", 
                (const char*)pkEmitter->GetName(), 
                (const char*)pkEmitter->GetRTTI()->GetName());
            m_wndEmittersList.SetItemText(nItem, 0, acString);

            nItem++;
        }
    }

    // Spawners list
    nItem = 0;
    m_wndSpawnersList.DeleteAllItems();
    uiCount = pkParticleSystem->GetSpawnerCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSSpawner* pkSpawner = pkParticleSystem->GetSpawnerAt(ui);
        if (pkSpawner)
        {
            NiSprintf(acString, 256, "%d", nItem);
            m_wndSpawnersList.InsertItem(nItem, acString);

            m_wndSpawnersList.SetItemText(nItem, 0,
                (const char*)pkSpawner->GetRTTI()->GetName());
            nItem++;
        }
    }

    // Death spawner button
    const NiPSSpawner* pkDeathSpawner = pkParticleSystem->GetDeathSpawner();
    if(pkDeathSpawner)
    {
        m_wndDeathSpawnerButton.EnableWindow(TRUE);
        NiSprintf(acString, 256, "0x%.8x", (long) PtrToLong(pkDeathSpawner));
    }
    else
    {
        m_wndDeathSpawnerButton.EnableWindow(FALSE);
        NiSprintf(acString, 256, "None");
    }
    m_wndDeathSpawnerButton.SetWindowText(acString);

    NiPSSimulator* pkSimulator = pkParticleSystem->GetSimulator();

    // Forces list
    nItem = 0;
    m_wndForcesList.DeleteAllItems();
    uiCount = pkSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSSimulatorForcesStep* pkForces = 
            NiDynamicCast(NiPSSimulatorForcesStep, pkSimulator->GetStepAt(ui));
        if (pkForces)
        {
            uiCount = pkForces->GetForcesCount();
            for (NiUInt32 ui = 0; ui < uiCount; ui++)
            {
                const NiPSForce* pkForce = pkForces->GetForceAt(ui);
                if (pkForce)
                {
                    NiSprintf(acString, 256, "%d", nItem);
                    m_wndForcesList.InsertItem(nItem, acString);

                    switch(pkForce->GetType())
                    {
                    case NiPSForceDefinitions::FORCE_BOMB:
                        NiSprintf(acString, 256, "%s (Bomb)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_DRAG:
                        NiSprintf(acString, 256, "%s (Drag)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_AIR_FIELD:
                        NiSprintf(acString, 256, "%s (Air Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_DRAG_FIELD:
                        NiSprintf(acString, 256, "%s (Drag Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_GRAVITY_FIELD:
                        NiSprintf(acString, 256, "%s (Gravity Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_RADIAL_FIELD:
                        NiSprintf(acString, 256, "%s (Radial Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_TURBULENCE_FIELD:
                        NiSprintf(acString, 256, "%s (Turbulence Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_VORTEX_FIELD:
                        NiSprintf(acString, 256, "%s (Vortex Field)", 
                            (const char*)pkForce->GetName());
                        break;
                    case NiPSForceDefinitions::FORCE_GRAVITY:
                        NiSprintf(acString, 256, "%s (Gravity)", 
                            (const char*)pkForce->GetName());
                        break;
                    default:
                        NiSprintf(acString, 256, "%s (Unknown)", 
                            (const char*)pkForce->GetName());
                        break;
                    }

                    m_wndForcesList.SetItemText(nItem, 0, acString);
                    nItem++;
                }
            }

            // Break out
            break;
        }
    }

    // Colliders list
    nItem = 0;
    m_wndCollidersList.DeleteAllItems();
    uiCount = pkSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSSimulatorCollidersStep* pkColliders = 
            NiDynamicCast(NiPSSimulatorCollidersStep, 
                pkSimulator->GetStepAt(ui));
        if (pkColliders)
        {
            uiCount = pkColliders->GetCollidersCount();
            for (NiUInt32 ui = 0; ui < uiCount; ui++)
            {
                const NiPSCollider* pkCollider = 
                    pkColliders->GetColliderAt(ui);
                if (pkCollider)
                {
                    NiSprintf(acString, 256, "%d", nItem);
                    m_wndCollidersList.InsertItem(nItem, acString);

                    switch(pkCollider->GetType())
                    {
                    case NiPSColliderDefinitions::COLLIDER_PLANAR:
                        NiSprintf(acString, 256, "Planar Collider");
                        break;
                    case NiPSColliderDefinitions::COLLIDER_SPHERICAL:
                        NiSprintf(acString, 256, "Planar Collider");
                        break;
                    default:
                        NiSprintf(acString, 256, "Unknown Collider");
                        break;
                    }
                    m_wndCollidersList.SetItemText(nItem, 0, acString);
                    nItem++;
                }
            }

            // Break out
            break;
        }
    }

    return true;

}
//---------------------------------------------------------------------------
BOOL CNiParticleSystemDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(NiIsKindOf(NiPSParticleSystem, m_pkObj));

    CRect rect;

    m_wndEmittersList.InsertColumn(0, "Name");
    m_wndEmittersList.GetWindowRect(&rect);
    m_wndEmittersList.SetColumnWidth(0, rect.Width()-4);

    m_wndSpawnersList.InsertColumn(0, "Name");
    m_wndSpawnersList.GetWindowRect(&rect);
    m_wndSpawnersList.SetColumnWidth(0, rect.Width()-4);

    m_wndForcesList.InsertColumn(0, "Name");
    m_wndForcesList.GetWindowRect(&rect);
    m_wndForcesList.SetColumnWidth(0, rect.Width()-4);

    m_wndCollidersList.InsertColumn(0, "Name");
    m_wndCollidersList.GetWindowRect(&rect);
    m_wndCollidersList.SetColumnWidth(0, rect.Width()-4);
  
    DoUpdate();
    return TRUE;  
    // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::OnDblclkEmittersList(NMHDR* pNMHDR, 
    LRESULT* pResult) 
{
    *pResult = 0;

    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return;
    
    if(m_wndEmittersList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndEmittersList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;

    NiUInt32 uiCount = pkParticleSystem->GetEmitterCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiPSEmitter* pkEmitter = pkParticleSystem->GetEmitterAt(ui);
        if (pkEmitter)
        {
            if(iMatchIndex == iIndex)
            {
                CNifPropertyWindowManager* pkManager = 
                    CNifPropertyWindowManager::GetPropertyWindowManager();

                pkManager->CreatePropertyWindow(pkEmitter);
                return;
            }

            iMatchIndex++;
        }
    }
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::OnDblclkSpawnersList(NMHDR* pNMHDR, 
    LRESULT* pResult) 
{
    *pResult = 0;

    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return;
    
    if(m_wndSpawnersList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndSpawnersList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;

    NiUInt32 uiCount = pkParticleSystem->GetSpawnerCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiPSSpawner* pkSpawner = pkParticleSystem->GetSpawnerAt(ui);
        if (pkSpawner)
        {
            if(iMatchIndex == iIndex)
            {
                CNifPropertyWindowManager* pkManager = 
                    CNifPropertyWindowManager::GetPropertyWindowManager();

                pkManager->CreatePropertyWindow(pkSpawner);
                return;
            }

            iMatchIndex++;
        }
    }
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::OnDeathSpawnerButton() 
{
    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return;

    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;
    if(pkParticleSystem->GetDeathSpawner())
    {
        CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkParticleSystem->GetDeathSpawner());
    }
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::OnDblclkForcesList(NMHDR* pNMHDR, 
    LRESULT* pResult) 
{
    *pResult = 0;

    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return;
    
    if(m_wndForcesList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndForcesList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;
    NiPSSimulator* pkSimulator = pkParticleSystem->GetSimulator();

    NiUInt32 uiCount = pkSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSSimulatorForcesStep* pkForces = 
            NiDynamicCast(NiPSSimulatorForcesStep, pkSimulator->GetStepAt(ui));
        if (pkForces)
        {
            uiCount = pkForces->GetForcesCount();
            for (NiUInt32 ui = 0; ui < uiCount; ui++)
            {
                NiPSForce* pkForce = pkForces->GetForceAt(ui);
                if (pkForce)
                {
                    if(iMatchIndex == iIndex)
                    {
                        CNifPropertyWindowManager* pkManager = 
                            CNifPropertyWindowManager::
                                GetPropertyWindowManager();

                        pkManager->CreatePropertyWindow(pkForce);
                        return;
                    }

                    iMatchIndex++;
                }
            }

            return;
        }
    }
}
//---------------------------------------------------------------------------
void CNiParticleSystemDlg::OnDblclkCollidersList(NMHDR* pNMHDR, 
    LRESULT* pResult) 
{
    *pResult = 0;

    if(!NiIsKindOf(NiPSParticleSystem, m_pkObj))
        return;
    
    if(m_wndCollidersList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndCollidersList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) m_pkObj;
    NiPSSimulator* pkSimulator = pkParticleSystem->GetSimulator();

    NiUInt32 uiCount = pkSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        const NiPSSimulatorCollidersStep* pkColliders = NiDynamicCast(
            NiPSSimulatorCollidersStep, pkSimulator->GetStepAt(ui));
        if (pkColliders)
        {
            uiCount = pkColliders->GetCollidersCount();
            for (NiUInt32 ui = 0; ui < uiCount; ui++)
            {
                NiPSCollider* pkCollider = pkColliders->GetColliderAt(ui);
                if (pkCollider)
                {
                    if(iMatchIndex == iIndex)
                    {
                        CNifPropertyWindowManager* pkManager = 
                            CNifPropertyWindowManager::
                                GetPropertyWindowManager();

                        pkManager->CreatePropertyWindow(pkCollider);
                        return;
                    }

                    iMatchIndex++;
                }
            }

            return;
        }
    }
}
//---------------------------------------------------------------------------