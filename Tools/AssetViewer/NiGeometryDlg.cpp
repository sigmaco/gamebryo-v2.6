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

// NiGeometryDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiGeometryDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiGeometryDlg dialog
//---------------------------------------------------------------------------
CNiGeometryDlg::CNiGeometryDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiGeometryDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiGeometryDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiGeometryDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiGeometryDlg)
    DDX_Control(pDX, IDC_NIGEOMETRY_SHADERNAME, m_wndShaderName);
    DDX_Control(pDX, IDC_NIGEOMETRY_NUM_SKIN_PARTITIONS,
        m_wndNumSkinPartitions);
    DDX_Control(pDX, IDC_NIGEOMETRY_GEOMETRYDATA_BUTTON, 
        m_wndGeometryDataButton);
    DDX_Control(pDX, IDC_NIGEOMETRY_SKIN_INSTANCE_BUTTON, 
        m_wndSkinInstanceButton);
    DDX_Control(pDX, IDC_NIGEOMETRY_VERTEX_COUNT_EDIT, 
        m_wndVertexCountEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_MODEL_BOUND_RADIUS_EDIT, 
        m_wndModelBoundRadiusEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_TEXTURE_SET_COUNT_EDIT, 
        m_wndTextureSetCountEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_MODEL_BOUND_CENTER_Z_EDIT, 
        m_wndModelBoundCenterZEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_MODEL_BOUND_CENTER_Y_EDIT, 
        m_wndModelBoundCenterYEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_MODEL_BOUND_CENTER_X_EDIT, 
        m_wndModelBoundCenterXEdit);
    DDX_Control(pDX, IDC_NIGEOMETRY_ACTIVE_VERTEX_COUNT_EDIT, 
        m_wndActiveVertCountEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiGeometryDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiGeometryDlg)
    ON_BN_CLICKED(IDC_NIGEOMETRY_SKIN_INSTANCE_BUTTON, 
        OnNigeometrySkinInstanceButton)
    ON_BN_CLICKED(IDC_NIGEOMETRY_GEOMETRYDATA_BUTTON, OnGeometryDataButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiGeometryDlg message handlers
//---------------------------------------------------------------------------
void CNiGeometryDlg::OnNigeometrySkinInstanceButton() 
{
    if(!NiIsKindOf(NiGeometry, m_pkObj))
        return;

    NiGeometry* pkGeom = (NiGeometry*) m_pkObj; 
    if(pkGeom->GetSkinInstance())
    {
        CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkGeom->GetSkinInstance());
    }
}
//---------------------------------------------------------------------------
void CNiGeometryDlg::OnGeometryDataButton() 
{
    if (!NiIsKindOf(NiGeometry, m_pkObj))
        return;

    NiGeometry* pkGeom = (NiGeometry*) m_pkObj;
    if (pkGeom->GetModelData())
    {
        CNifPropertyWindowManager* pkManager =
            CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkGeom->GetModelData());
    }
}
//---------------------------------------------------------------------------
bool CNiGeometryDlg::DoUpdate()
{
    if(!NiIsKindOf(NiGeometry, m_pkObj))
        return false;

    NiGeometry* pkGeom = (NiGeometry*) m_pkObj;
    char acString[256];

    NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
    if(pkSkinInstance)
    {
        m_wndSkinInstanceButton.EnableWindow(TRUE);
        NiSprintf(acString, 256, "0x%.8x", (long) PtrToLong(pkSkinInstance));
    }
    else
    {
        m_wndSkinInstanceButton.EnableWindow(FALSE);
        NiSprintf(acString, 256, "None");
    }
    m_wndSkinInstanceButton.SetWindowText(acString);

    if (pkSkinInstance && pkSkinInstance->GetSkinPartition())
    {
        NiSprintf(acString, 256, "%d", pkSkinInstance->GetSkinPartition()
            ->GetPartitionCount());
    }
    else
    {
        NiSprintf(acString, 256, "N/A");
    }
    m_wndNumSkinPartitions.SetWindowText(acString);
    
    const NiMaterial* pkMaterial = pkGeom->GetActiveMaterial();
    if (pkMaterial)
        NiSprintf(acString, 256, "%s", (const char*)pkMaterial->GetName());
    else
        NiSprintf(acString, 256, "Default Fixed-Function Pipeline");

    m_wndShaderName.SetWindowText(acString);

    NiSprintf(acString, 256, "%d", pkGeom->GetVertexCount());
    m_wndVertexCountEdit.SetWindowText(acString);

    NiBound kBound = pkGeom->GetModelBound();
    NiSprintf(acString, 256, "%.4f", kBound.GetRadius());
    m_wndModelBoundRadiusEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%d", pkGeom->GetTextureSets());
    m_wndTextureSetCountEdit.SetWindowText(acString);

    NiPoint3 kPoint = kBound.GetCenter();
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndModelBoundCenterZEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndModelBoundCenterYEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndModelBoundCenterXEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%d", pkGeom->GetActiveVertexCount());
    m_wndActiveVertCountEdit.SetWindowText(acString);

    if (pkGeom->GetModelData())
    {
        m_wndGeometryDataButton.EnableWindow(TRUE);
        NiSprintf(acString, 256, "0x%.8x", (long) PtrToLong(pkGeom->GetModelData()));
    }
    else
    {
        m_wndGeometryDataButton.EnableWindow(FALSE);
        NiSprintf(acString, 256, "None");
    }
    m_wndGeometryDataButton.SetWindowText(acString);

    return true;
}
//---------------------------------------------------------------------------
