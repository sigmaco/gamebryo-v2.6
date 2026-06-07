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

// NiTexturingPropertyDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTexturingPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiTexturingPropertyDlg dialog
//---------------------------------------------------------------------------
CNiTexturingPropertyDlg::CNiTexturingPropertyDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiTexturingPropertyDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiTexturingPropertyDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiTexturingPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiTexturingPropertyDlg)
    DDX_Control(pDX, IDC_TEXTURINGPROP_TEXTURENAME_EDIT, 
        m_wndTextureNameEdit);
    DDX_Control(pDX, IDC_TEXTURINGPROP_TEXTURECLASS_EDIT, 
        m_wndTextureClassEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_WIDTH_EDIT, m_wndWidthEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_TEXTURESET_EDIT, 
        m_wndTextureSetEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_PIXELLAYOUT_COMBO, 
        m_wndPixelLayoutCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MIPMAPMODE_COMBO, 
        m_wndMipMapCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MATRIX11_EDIT,
        m_wndMatrix11Edit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MATRIX10_EDIT, 
        m_wndMatrix10Edit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MATRIX01_EDIT, 
        m_wndMatrix01Edit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MATRIX00_EDIT, 
        m_wndMatrix00Edit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_MAPLIST, m_wndMapList);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_LUMAOFFSET_EDIT,
        m_wndLumaOffsetEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_LUMA_SCALE_EDIT, 
        m_wndLumaScaleEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_HEIGHT_EDIT, m_wndHeightEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_FILTERMODE_COMBO, 
        m_wndFilterModeCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_CLAMPMODE_COMBO, 
        m_wndClampModeCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_APPLYMODE_COMBO, 
        m_wndApplyModeCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_PLATFORM_COMBO, 
        m_wndPlatformCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_ALPHAFORMAT_COMBO, 
        m_wndAlphaFormatCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_TRANSFORM_TYPE_COMBO, 
        m_wndTransformTypeCombo);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_CENTER_U_EDIT, 
        m_wndCenterUOffsetEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_CENTER_V_EDIT, 
        m_wndCenterVOffsetEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_SCALE_U_EDIT, m_wndTilingUEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_SCALE_V_EDIT, m_wndTilingVEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_TRANSLATE_U_EDIT, 
        m_wndTranslateUEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_TRANSLATE_V_EDIT,
        m_wndTranslateVEdit);
    DDX_Control(pDX, IDC_NITEXTURINGPROPERTY_ROTATE_W_EDIT,
        m_wndWRotationEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiTexturingPropertyDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiTexturingPropertyDlg)
    ON_NOTIFY(NM_CLICK, IDC_NITEXTURINGPROPERTY_MAPLIST, OnClkMapList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiTexturingPropertyDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiTexturingPropertyDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    if(!NiIsKindOf(NiTexturingProperty, m_pkObj))
        return FALSE;

    m_wndMapList.InsertColumn(0, "Map:");
    CRect rect;
    m_wndMapList.GetWindowRect(&rect);
    int cx = rect.Width();
    m_wndMapList.SetColumnWidth(0, cx-4);

    RebuildMapList();
    m_wndMapList.SetItemState(0,LVIS_SELECTED, LVIS_SELECTED);   

    m_wndFilterModeCombo.AddString("FILTER_NEAREST");
    m_wndFilterModeCombo.AddString("FILTER_BILERP");
    m_wndFilterModeCombo.AddString("FILTER_TRILERP");
    m_wndFilterModeCombo.AddString("FILTER_NEAREST_MIPNEAREST");
    m_wndFilterModeCombo.AddString("FILTER_NEAREST_MIPLERP");
    m_wndFilterModeCombo.AddString("FILTER_BILERP_MIPNEAREST");

    m_wndClampModeCombo.AddString("CLAMP_S_CLAMP_T");
    m_wndClampModeCombo.AddString("CLAMP_S_WRAP_T");
    m_wndClampModeCombo.AddString("WRAP_S_CLAMP_T");
    m_wndClampModeCombo.AddString("WRAP_S_WRAP_T");

    m_wndApplyModeCombo.AddString("APPLY_REPLACE");
    m_wndApplyModeCombo.AddString("APPLY_DECAL");
    m_wndApplyModeCombo.AddString("APPLY_MODULATE");

    m_wndAlphaFormatCombo.AddString("NONE");
    m_wndAlphaFormatCombo.AddString("BINARY");
    m_wndAlphaFormatCombo.AddString("SMOOTH");
    m_wndAlphaFormatCombo.AddString("ALPHA_DEFAULT");

    m_wndPixelLayoutCombo.AddString("PALETTIZED");
    m_wndPixelLayoutCombo.AddString("HIGH_COLOR_16");
    m_wndPixelLayoutCombo.AddString("TRUE_COLOR_32");
    m_wndPixelLayoutCombo.AddString("COMPRESSED");
    m_wndPixelLayoutCombo.AddString("BUMPMAP");
    m_wndPixelLayoutCombo.AddString("PALETTIZED_4");
    m_wndPixelLayoutCombo.AddString("PIX_DEFAULT");

    m_wndMipMapCombo.AddString("NO");
    m_wndMipMapCombo.AddString("YES");
    m_wndMipMapCombo.AddString("MIP_DEFAULT");

    m_wndTransformTypeCombo.AddString("NONE");
    m_wndTransformTypeCombo.AddString("MAX");
    m_wndTransformTypeCombo.AddString("MAYA");

    m_wndPlatformCombo.AddString("Generic");
    m_wndPlatformCombo.AddString("Xbox360");
    m_wndPlatformCombo.AddString("Playstation3");
    m_wndPlatformCombo.AddString("DX9");
    m_wndPlatformCombo.AddString("Wii");
    m_wndPlatformCombo.AddString("D3D10");

    DoUpdate();

    return TRUE;
}
//---------------------------------------------------------------------------
bool CNiTexturingPropertyDlg::DoUpdate()
{
    if( m_wndMapList.GetSelectedCount() == 0)
        return false;

    char acString[512];
    int iIndex = m_wndMapList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1 || !NiIsKindOf(NiTexturingProperty, m_pkObj))
        return false;

    NiTexturingProperty* pkProp = (NiTexturingProperty*) m_pkObj;

    NiTexturingProperty::Map* pkMap = 
        m_kMapArray.GetAt((unsigned int)iIndex);
    
    if(!pkMap)
        return false;

    // Basic info
    NiTexture* pkTexture = pkMap->GetTexture();
    if(!pkTexture)
        return false;

    NiSprintf(acString, 512,  "%s", pkTexture->GetRTTI()->GetName());
    m_wndTextureClassEdit.SetWindowText(acString);

    NiSprintf(acString, 512, "%d", pkMap->GetTextureIndex());
    m_wndTextureSetEdit.SetWindowText(acString);

    if(NiIsKindOf(NiSourceTexture, pkTexture))
    {
        NiSourceTexture* pkSourceTexture = 
            (NiSourceTexture*) pkTexture;

        if(pkSourceTexture->GetFilename())
            NiSprintf(acString, 512, "%s", pkSourceTexture->GetFilename());
        else
            NiSprintf(acString, 512, "0x%.8x", (long) PtrToLong(pkTexture));
    }
    else if(pkTexture != NULL)
        NiSprintf(acString, 512, "0x%.8x", (long) PtrToLong(pkTexture));

    m_wndTextureNameEdit.SetWindowText(acString);
    
    NiSprintf(acString, 512, "%d", pkTexture->GetWidth());
    m_wndWidthEdit.SetWindowText(acString);
    NiSprintf(acString, 512, "%d", pkTexture->GetHeight());
    m_wndHeightEdit.SetWindowText(acString);

    int iPixelLayout = (int)
        pkTexture->GetFormatPreferences().m_ePixelLayout;
    m_wndPixelLayoutCombo.SetCurSel(iPixelLayout);
    m_wndMipMapCombo.SetCurSel((int)
        pkTexture->GetFormatPreferences().m_eMipMapped);
    m_wndAlphaFormatCombo.SetCurSel((int)
        pkTexture->GetFormatPreferences().m_eAlphaFmt);

    m_wndFilterModeCombo.SetCurSel((int)pkMap->GetFilterMode());
    m_wndClampModeCombo.SetCurSel((int)pkMap->GetClampMode());
    m_wndApplyModeCombo.SetCurSel((int)pkProp->GetApplyMode());

    NiSourceTexture* pkSrcTxtr =
        NiDynamicCast(NiSourceTexture, pkMap->GetTexture());
    if (pkSrcTxtr)
    {
        const NiPersistentSrcTextureRendererData* pkPSTRD =
            pkSrcTxtr->GetPersistentSourceRendererData();
        if (pkPSTRD)
            m_wndPlatformCombo.SetCurSel((int)pkPSTRD->GetTargetPlatform());
        else
            m_wndPlatformCombo.SetCurSel(0);
    }
    else
    {
        m_wndPlatformCombo.SetCurSel(0);
    }
    
    if(iIndex == m_iBumpMapIndex)
    {
        NiTexturingProperty::BumpMap* pkBumpMap = 
            (NiTexturingProperty::BumpMap*) pkMap;
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetBumpMat11());
        m_wndMatrix11Edit.SetWindowText(acString);
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetBumpMat10());
        m_wndMatrix10Edit.SetWindowText(acString);
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetBumpMat01());
        m_wndMatrix01Edit.SetWindowText(acString);
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetBumpMat00());
        m_wndMatrix00Edit.SetWindowText(acString);
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetLumaOffset());
        m_wndLumaOffsetEdit.SetWindowText(acString);
        NiSprintf(acString, 512, "%.4f", pkBumpMap->GetLumaScale());
        m_wndLumaScaleEdit.SetWindowText(acString);
    }
    else
    {
        NiSprintf(acString, 512, "N/A");
        m_wndMatrix11Edit.SetWindowText(acString);
        m_wndMatrix10Edit.SetWindowText(acString);
        m_wndMatrix01Edit.SetWindowText(acString);
        m_wndMatrix00Edit.SetWindowText(acString);
        m_wndLumaOffsetEdit.SetWindowText(acString);
        m_wndLumaScaleEdit.SetWindowText(acString);
    }

    NiTextureTransform* pkTransform = pkMap->GetTextureTransform();
    if (pkTransform)
    {
        m_wndTransformTypeCombo.SetCurSel(
            (int)pkTransform->GetTransformMethod());

        NiSprintf(acString, 512, "%f", pkTransform->GetCenter().x);
        m_wndCenterUOffsetEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetCenter().y);
        m_wndCenterVOffsetEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetScale().x);
        m_wndTilingUEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetScale().y);
        m_wndTilingVEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetTranslate().x);
        m_wndTranslateUEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetTranslate().y);
        m_wndTranslateVEdit.SetWindowText(acString);;
        
        NiSprintf(acString, 512, "%f", pkTransform->GetRotate());
        m_wndWRotationEdit.SetWindowText(acString);;
    }
    else
    {
        m_wndTransformTypeCombo.SetCurSel(2);

        m_wndCenterUOffsetEdit.SetWindowText("");;
        m_wndCenterVOffsetEdit.SetWindowText("");;
        m_wndTilingUEdit.SetWindowText("");;
        m_wndTilingVEdit.SetWindowText("");;
        m_wndTranslateUEdit.SetWindowText("");;
        m_wndTranslateVEdit.SetWindowText("");;
        m_wndWRotationEdit.SetWindowText("");;
    }

    return true;
}
//---------------------------------------------------------------------------
void CNiTexturingPropertyDlg::RebuildMapList()
{
    if(!NiIsKindOf(NiTexturingProperty, m_pkObj))
        return;

    m_wndMapList.DeleteAllItems();
    m_iBumpMapIndex = -1;
    NiTexturingProperty* pkProp = (NiTexturingProperty*) m_pkObj;

    unsigned int uiSize = pkProp->GetMaps().GetEffectiveSize() +
        pkProp->GetDecalArrayCount() + pkProp->GetShaderArrayCount();

    m_kMapArray.SetSize(uiSize);

    int nItem = 0;

    if(pkProp->GetBaseMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetBaseMap());
        m_wndMapList.InsertItem(nItem++, "Base Map");
    }

    if(pkProp->GetDarkMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetDarkMap());
        m_wndMapList.InsertItem(nItem++, "Dark Map");
    }

    if(pkProp->GetDetailMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetDetailMap());
        m_wndMapList.InsertItem(nItem++, "Detail Map");
    }

    if(pkProp->GetGlossMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetGlossMap());
        m_wndMapList.InsertItem(nItem++, "Gloss Map");
    }

    if(pkProp->GetGlowMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetGlowMap());
        m_wndMapList.InsertItem(nItem++, "Glow Map");
    }

    if(pkProp->GetBumpMap())
    {
        m_iBumpMapIndex = nItem;
        m_kMapArray.SetAt(nItem, pkProp->GetBumpMap());
        m_wndMapList.InsertItem(nItem++, "Bump Map");
    }

    if(pkProp->GetNormalMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetNormalMap());
        m_wndMapList.InsertItem(nItem++, "Normal Map");
    }

    if(pkProp->GetParallaxMap())
    {
        m_kMapArray.SetAt(nItem, pkProp->GetParallaxMap());
        m_wndMapList.InsertItem(nItem++, "Parallax Map");
    }

    int iDecal = 0;
    unsigned int ui;
    for(ui = 0; ui < pkProp->GetDecalArrayCount();ui++)
    {
        if(pkProp->GetDecalMap(ui))
        {
            char acString[256];
            m_kMapArray.SetAt(nItem, pkProp->GetDecalMap(ui));
            NiSprintf(acString, 256, "Decal Map %d", iDecal++);
            m_wndMapList.InsertItem(nItem++, acString);
        }
    }

    int iShader = 0;
    for(ui = 0; ui < pkProp->GetShaderArrayCount(); ui++)
    {
        if(pkProp->GetShaderMap(ui))
        {
            char acString[256];
            m_kMapArray.SetAt(nItem, pkProp->GetShaderMap(ui));
            NiSprintf(acString, 256, "Shader Map %d", ui);
            m_wndMapList.InsertItem(nItem++, acString);
        }
    }
}
//---------------------------------------------------------------------------
void CNiTexturingPropertyDlg::OnClkMapList(NMHDR* pNMHDR, LRESULT* pResult)
{
    DoUpdate();
}
//---------------------------------------------------------------------------
