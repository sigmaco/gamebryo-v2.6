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

// NiGeometryDataAttributesInfoDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiGeometryDataAttributesInfoDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiGeometryDataAttributesInfoDlg dialog
//---------------------------------------------------------------------------
CNiGeometryDataAttributesInfoDlg::CNiGeometryDataAttributesInfoDlg(
    CWnd* pParent /*=NULL*/) : CNiObjectDlg(
        CNiGeometryDataAttributesInfoDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiGeometryDataAttributesInfoDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiGeometryDataAttributesInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiGeometryDataAttributesInfoDlg)
    DDX_Control(pDX, IDC_NIGEOMETRY_CONSISTENCY, m_wndConsistencyEdit);
    DDX_Control(pDX, IDC_KEEP_XYZ_POSITIONS, m_wndKeepPositions);
    DDX_Control(pDX, IDC_KEEP_NORMALS, m_wndKeepNormals);
    DDX_Control(pDX, IDC_KEEP_COLORS, m_wndKeepColors);
    DDX_Control(pDX, IDC_KEEP_UVS, m_wndKeepUVs);
    DDX_Control(pDX, IDC_KEEP_INDICES, m_wndKeepIndices);
    DDX_Control(pDX, IDC_KEEP_BONE_DATA, m_wndKeepBoneData);
    DDX_Control(pDX, IDC_COMPRESS_XYZ_POSITION, m_wndCompressPosition);
    DDX_Control(pDX, IDC_COMPRESS_NORMALS, m_wndCompressNormals);
    DDX_Control(pDX, IDC_COMPRESS_COLORS, m_wndCompressColors);
    DDX_Control(pDX, IDC_COMPRESS_UVs, m_wndCompressUVs);
    DDX_Control(pDX, IDC_COMPRESS_WEIGHTS, m_wndCompressWeights);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiGeometryDataAttributesInfoDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiGeometryDataAttributesInfoDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiGeometryDataAttributesInfoDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiGeometryDataAttributesInfoDlg::OnInitDialog() 
{
    CNiObjectDlg::OnInitDialog();
    
    DoUpdate();

    return TRUE;
}
//---------------------------------------------------------------------------
bool CNiGeometryDataAttributesInfoDlg::DoUpdate()
{
    if(m_pkObj == NULL)
        return false;

    NiGeometryData* pkData = (NiGeometryData*)m_pkObj;

    switch (pkData->GetConsistency())
    {
    case NiGeometryData::MUTABLE:
        m_wndConsistencyEdit.SetWindowText("Mutable");
        break;
    case NiGeometryData::STATIC:
        m_wndConsistencyEdit.SetWindowText("Static");
        break;
    case NiGeometryData::VOLATILE:
        m_wndConsistencyEdit.SetWindowText("Volatile");
        break;
    }

    m_wndKeepPositions.SetCheck(pkData->GetKeepFlags() &
        NiGeometryData::KEEP_XYZ);
    m_wndKeepNormals.SetCheck(pkData->GetKeepFlags() &
        NiGeometryData::KEEP_NORM);
    m_wndKeepColors.SetCheck(pkData->GetKeepFlags() &
        NiGeometryData::KEEP_COLOR);
    m_wndKeepUVs.SetCheck(pkData->GetKeepFlags() & 
        NiGeometryData::KEEP_UV);
    m_wndKeepIndices.SetCheck(pkData->GetKeepFlags() & 
        NiGeometryData::KEEP_INDICES);
    m_wndKeepBoneData.SetCheck(pkData->GetKeepFlags() & 
        NiGeometryData::KEEP_BONEDATA);
    m_wndCompressPosition.SetCheck(pkData->GetCompressFlags() &
        NiGeometryData::COMPRESS_POSITION);
    m_wndCompressNormals.SetCheck(pkData->GetCompressFlags() & 
        NiGeometryData::COMPRESS_NORM);
    m_wndCompressColors.SetCheck(pkData->GetCompressFlags() &
        NiGeometryData::COMPRESS_COLOR);
    m_wndCompressUVs.SetCheck(pkData->GetCompressFlags() &
        NiGeometryData::COMPRESS_UV);
    m_wndCompressWeights.SetCheck(pkData->GetCompressFlags() & 
        NiGeometryData::COMPRESS_WEIGHT);

    return true;
}
//---------------------------------------------------------------------------
