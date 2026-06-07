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

#include "TerrainPluginPCH.h"

#include "MBrushType.h"
#include "MTerrainPlugin.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

void MBrushType::SetSizeInner(float fSize)
{
    m_fSizeInner = fSize;
    m_fSizeInnerSqr = NiSqr(fSize);
    
    float fScale = MTerrainPlugin::GetInstance()->GetBrush()->GetScale();
    ResizeBrushOverlay(fScale);
}
//---------------------------------------------------------------------------
void MBrushType::SetSizeOuter(float fSize)
{ 
    // m_fSizeOuter should always be >= m_fSizeInner
    if (fSize < m_fSizeInner)
    {
        NiMessageBox("Outer radius not greater than Active Radius", 
            "AeBrush::SetSizeOuter");
        return;
    }

    m_fSizeOuter = fSize;
    m_fSizeOuterSqr = NiSqr(fSize);

    float fScale = MTerrainPlugin::GetInstance()->GetBrush()->GetScale();
    ResizeBrushOverlay(fScale);
}
//---------------------------------------------------------------------------
float MBrushType::GetSizeInner()
{
    return m_fSizeInner;
}
//---------------------------------------------------------------------------
float MBrushType::GetSizeOuter()
{
    return m_fSizeOuter;
}

//===========================================================================
// Brush Overlay
//===========================================================================

NiMesh* MBrushType::GetBrushOverlay() 
{
    return m_pkBrushOverlay;
}
//---------------------------------------------------------------------------
void MBrushType::SetColor(NiColor kColor)
{
    NiColorA kSetTo = NiColorA(kColor.r , kColor.g, kColor.b, 255.0f);
    if (*m_pkBaseColor != kSetTo)
    {
        *m_pkBaseColor = kSetTo;
        
        NiDelete (m_pkBrushOverlay);
        m_pkBrushOverlay = 0;
    }
}
//---------------------------------------------------------------------------
void MBrushType::SetColor(NiColorA kColor)
{
    if (*m_pkBaseColor != kColor)
    {
        *m_pkBaseColor = NiColorA(kColor.r , kColor.g, kColor.b, kColor.a);
        
        NiDelete (m_pkBrushOverlay);
        m_pkBrushOverlay = 0;
    }
}
//---------------------------------------------------------------------------
void MBrushType::SetColor(String* pkColorName)
{
    if (String::Compare(pkColorName,"Brush allowed color")==0)
    {
        SetColor(*m_pkAllowedColor);
    }
    else if (String::Compare(pkColorName,"Brush warning color")==0)
    {
        SetColor(*m_pkWarningColor);
    }
    else if (String::Compare(pkColorName,"Brush stop color")==0)
    {
        SetColor(*m_pkStopColor);
    }
}
//---------------------------------------------------------------------------
bool MBrushType::ValidateBrushType()
{
    if (m_fSizeInner <= 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void MBrushType::RegisterBrushTypeOptions()
{
    if (!m_pkBaseColor)
    {
        // Colors haven't been initialised for this brush type
        // Set default values.
        m_pkAllowedColor = NiNew NiColorA(0.41176f, 1.0f, 0.70588f, 1.0f);
        m_pkWarningColor = NiNew NiColorA(1.0f, 1.0f, 0.70588f, 1.0f);
        m_pkStopColor = NiNew NiColorA(1.0f, 0.41176f, 0.70588f, 1.0f);

        m_pkBaseColor = NiNew NiColorA(m_pkAllowedColor->r, m_pkAllowedColor->g,
            m_pkAllowedColor->b, m_pkAllowedColor->a);
    }
    else
    {
        // we already setup those options, no need to do it again
        return;
    }

    // Those are options used by all brush types
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MBrushType::OnColorSettingChanged);

    RegisterCustomSettings("Terrain editing options.Brush allowed color",
        "Brush allowed color", 
        "Color of the brush when painting is available",
        *m_pkAllowedColor, pmHandler);
    RegisterCustomSettings("Terrain editing options.Brush warning color",
        "Brush warning color", 
        "Color of the brush when the painting stroke is getting closer to the "
        "limitation",
        *m_pkWarningColor, pmHandler);
    RegisterCustomSettings("Terrain editing options.Brush stop color",
        "Brush stop color", 
        "Color of the brush when painting is no longer available available",
        *m_pkStopColor, pmHandler);

    // Set initial brush color
    SetColor("Brush allowed color");
}
//---------------------------------------------------------------------------
void MBrushType::RegisterCustomSettings(String* pmOptionCategoryName,
    String* pmOptionName, 
    String* pmOptionDescription,
    NiColorA& kValue,
    SettingChangedHandler* pmHandler)
{
    ISettingsService* pmSettingService = MGetService(ISettingsService);
    // first register the setting in case it doesn't already exist
    pmSettingService->RegisterSettingsObject(pmOptionName, 
        __box(Color::FromArgb(255,
        MUtility::FloatToRGB(kValue.r), 
        MUtility::FloatToRGB(kValue.g), 
        MUtility::FloatToRGB(kValue.b))), 
        SettingsCategory::PerUser);
    // set the event handler for when that setting changes

    if (pmHandler != NULL)
    {
        pmSettingService->SetChangedSettingHandler(pmOptionName, 
            SettingsCategory::PerUser, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    __box Color* pmValue;
    pmObj = pmSettingService->GetSettingsObject(pmOptionName, 
        SettingsCategory::PerUser);

    IOptionsService* pmOptionService = MGetService(IOptionsService);

    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        kValue.r = MUtility::RGBToFloat((*pmValue).R);
        kValue.g = MUtility::RGBToFloat((*pmValue).G);
        kValue.b = MUtility::RGBToFloat((*pmValue).B);
     
        pmOptionService->AddOption(pmOptionCategoryName, 
            SettingsCategory::PerUser, pmOptionName); 
        
        pmOptionService->SetHelpDescription(pmOptionCategoryName, 
            pmOptionDescription);   
    }
}
//---------------------------------------------------------------------------
void MBrushType::OnColorSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    NI_UNUSED_ARG(pmSender);

    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    ISettingsService* pmSettingsService = MGetService(
        ISettingsService);

    Object* pmObj = pmSettingsService->GetSettingsObject(
            strSetting, eCategory);
    __box Color* pmValue;
    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        if (String::Compare(strSetting,"Brush allowed color")==0)
        {
            m_pkAllowedColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkAllowedColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkAllowedColor->b = MUtility::RGBToFloat((*pmValue).B);

            // make sure we change the brush color when changing the setting
            SetColor("Brush allowed color");
        }
        else if (String::Compare(strSetting,"Brush warning color")==0)
        {
            m_pkWarningColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkWarningColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkWarningColor->b = MUtility::RGBToFloat((*pmValue).B);
        }
        else if (String::Compare(strSetting,"Brush stop color")==0)
        {
            m_pkStopColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkStopColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkStopColor->b = MUtility::RGBToFloat((*pmValue).B);
        }
        
    }
}
//---------------------------------------------------------------------------
void MBrushType::Do_Dispose(bool bDisposing)
{
    NI_UNUSED_ARG(bDisposing);

    NiDelete(m_pkBrushOverlay);
    m_pkBrushOverlay = 0;

    NiDelete(m_pkBaseColor);
    m_pkBaseColor = 0;

    NiDelete(m_pkAllowedColor);
    m_pkAllowedColor = 0;

    NiDelete(m_pkWarningColor);
    m_pkWarningColor = 0;

    NiDelete(m_pkStopColor);
    m_pkStopColor = 0;
}
