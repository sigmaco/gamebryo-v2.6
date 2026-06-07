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

// NifUserPreferences.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifUserPreferences.h"
#include "NifKeyboardShortcuts.h"

bool CNifUserPreferences::ms_bBroadcastChanged = false;
CCriticalSection CNifUserPreferences::ms_kCriticalSection;
unsigned long CNifUserPreferences::ms_ulThreadId = 0;
CNifUserPreferencesPtr CNifUserPreferences::ms_spThis = 0;
bool CNifUserPreferences::ms_bWorking = false;
bool CNifUserPreferences::ms_bSaveOnExit = true;

//---------------------------------------------------------------------------
void CNifUserPreferences::Lock()
{ 
    ms_bWorking = ms_kCriticalSection.Lock()? true:false;
    ms_ulThreadId = GetCurrentThreadId();
}
//---------------------------------------------------------------------------
CNifUserPreferences* CNifUserPreferences::AccessUserPreferences()
{ 
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    if(!ms_spThis)
    {
        ms_spThis = NiNew CNifUserPreferences();
        Load();
    }
    return ms_spThis;
}
//---------------------------------------------------------------------------
void CNifUserPreferences::UnLock()
{ 
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    if (ms_spThis && ms_bBroadcastChanged)
    {
        ms_spThis->BroadcastChanged();
    }

    ms_bWorking = !(ms_kCriticalSection.Unlock() ? true : false);
    if(ms_kCriticalSection.m_sect.LockCount == -1)
        ms_ulThreadId = NULL;
}
//---------------------------------------------------------------------------
void CNifUserPreferences::Destroy()
{
    Lock();
    if(ms_bSaveOnExit)
        Save();
    ms_spThis = 0;
    UnLock();
}
//---------------------------------------------------------------------------
void CNifUserPreferences::Save()
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    if (!ms_spThis)
    {
        return;
    }

    ms_spThis->m_kKeyboardShortcuts.Save();

    theApp.SetSettingsParamFloat("Translate Speed [x]",
        ms_spThis->GetTranslateSpeed().x);
    theApp.SetSettingsParamFloat("Translate Speed [y]",
        ms_spThis->GetTranslateSpeed().y);
    theApp.SetSettingsParamFloat("Translate Speed [z]",
        ms_spThis->GetTranslateSpeed().z);
    theApp.SetSettingsParamFloat("AnimationTickRate",
        ms_spThis->GetAnimationSliderSecondsPerTick());

    theApp.SetSettingsParamFloat("Yaw Speed", ms_spThis->GetYawSpeed());

    theApp.SetSettingsParamFloat("Pitch Speed", ms_spThis->GetPitchSpeed());

    NiColor kBackgroundColor = ms_spThis->GetBackgroundColor();
    COLORREF color = RGB(
        (BYTE) (kBackgroundColor.r * 255),
        (BYTE) (kBackgroundColor.g * 255),
        (BYTE) (kBackgroundColor.b * 255));
    theApp.SetSettingsParamInt("Background Color", color);

    theApp.SetSettingsParamInt("Use Default Lights",
        ms_spThis->GetUseDefaultLights());

    NiPoint3 kUpAxis = ms_spThis->GetUpAxis();
    CString strUpAxis;
    if (kUpAxis == NiPoint3::UNIT_Y)
    {
        strUpAxis = "y";
    }
    else    // if (kUpAxis == NiPoint3::UNIT_Z)
    {
        strUpAxis = "z";
    }
    if (!strUpAxis.IsEmpty())
    {
        theApp.SetSettingsParamString("Up Axis", strUpAxis);
    }

    theApp.SetSettingsParamString("CollisionLabPath",
        ms_spThis->m_strCollisionLabPath);

    theApp.SetSettingsParamString("ImageSubfolderPath",
        ms_spThis->m_strImageSubfolderPath);

    theApp.SetSettingsParamInt("Prompt On Discard",
        ms_spThis->GetPromptOnDiscard());

    theApp.SetSettingsParamInt("Highlight Selected",
        ms_spThis->GetHighlightSelected());

    theApp.SetSettingsParamInt("Show ABVs", ms_spThis->GetShowABVs());

    theApp.SetSettingsParamInt("Camera Control Mode",
        static_cast<int>(ms_spThis->GetCameraControlMode()));
}
//---------------------------------------------------------------------------
void CNifUserPreferences::Load()
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    if (!ms_spThis)
    {
        return;
    }

    ms_spThis->m_kKeyboardShortcuts.Load();

    NiPoint3 kValue;
    kValue.x = theApp.GetSettingsParamFloat("Translate Speed [x]");
    kValue.y = theApp.GetSettingsParamFloat("Translate Speed [y]");
    kValue.z = theApp.GetSettingsParamFloat("Translate Speed [z]");
    if (kValue != NiPoint3(g_fParamNotFound, g_fParamNotFound,
        g_fParamNotFound))
    {
        ms_spThis->SetTranslateSpeed(kValue);
    }

    float fValue = theApp.GetSettingsParamFloat("AnimationTickRate");
    if (fValue != g_fParamNotFound)
    {
        ms_spThis->SetAnimationSliderSecondsPerTick(fValue);
    }

    fValue = theApp.GetSettingsParamFloat("Yaw Speed");
    if (fValue != g_fParamNotFound)
    {
        ms_spThis->SetYawSpeed(fValue);
    }

    fValue = theApp.GetSettingsParamFloat("Pitch Speed");
    if (fValue != g_fParamNotFound)
    {
        ms_spThis->SetPitchSpeed(fValue);
    }

    COLORREF color = theApp.GetSettingsParamInt("Background Color");
    if ((int) color != g_iParamNotFound)
    {
        ms_spThis->SetBackgroundColor(NiColor(
            (float) GetRValue(color) / 255,
            (float) GetGValue(color) / 255,
            (float) GetBValue(color) / 255));
    }

    int iValue = theApp.GetSettingsParamInt("Use Default Lights");
    if (iValue != g_iParamNotFound)
    {
        ms_spThis->SetUseDefaultLights(iValue == 1 ? true : false);
    }

    ms_spThis->m_strCollisionLabPath = 
        theApp.GetSettingsParamString("CollisionLabPath");

    ms_spThis->m_strImageSubfolderPath = 
        theApp.GetSettingsParamString("ImageSubfolderPath");

    CString strValue = theApp.GetSettingsParamString("Up Axis");
    if (strValue != g_strParamNotFound)
    {
        NiPoint3 kUpAxis;
        if (strValue == "y")
        {
            kUpAxis = NiPoint3::UNIT_Y;
        }
        else    // if (strValue == "z")
        {
            kUpAxis = NiPoint3::UNIT_Z;
        }
        ms_spThis->SetUpAxis(kUpAxis);
    }

    iValue = theApp.GetSettingsParamInt("Prompt On Discard");
    if (iValue != g_iParamNotFound)
    {
        ms_spThis->SetPromptOnDiscard(iValue == 1 ? true : false);
    }

    iValue = theApp.GetSettingsParamInt("Highlight Selected");
    if (iValue != g_iParamNotFound)
    {
        ms_spThis->SetHighlightSelected(iValue == 1 ? true : false);
    }

    iValue = theApp.GetSettingsParamInt("Show ABVs");
    if (iValue != g_iParamNotFound)
    {
        ms_spThis->SetShowABVs(iValue == 1 ? true : false);
    }

    iValue = theApp.GetSettingsParamInt("Camera Control Mode");
    if (iValue != g_iParamNotFound)
    {
        ms_spThis->SetCameraControlMode(
            static_cast<CNifUserPreferences::CameraControlMode>(iValue));
    }

    ms_bBroadcastChanged = false;
}
//---------------------------------------------------------------------------
void CNifUserPreferences::BroadcastChanged()
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->UpdateAllViews(NULL, 
            MAKELPARAM(NIF_USERPREFERENCESCHANGED, 0), NULL);
    }
    ms_bBroadcastChanged = false;
}
//---------------------------------------------------------------------------
