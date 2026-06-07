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

// NifUserPreferences.inl

#include "NifDoc.h"

//---------------------------------------------------------------------------
inline CNifUserPreferences::CNifUserPreferences() :
    m_fAnimSecondsPerTick(1.0f),
    m_kTranslateSpeed(NiPoint3::UNIT_ALL * 0.5f),
    m_fYawSpeed(NI_PI / 8.0f), m_fPitchSpeed(NI_PI / 8.0f),
    m_kBackgroundColor(NiColor(0.5f, 0.5f, 0.5f)),
    m_bUseDefaultLights(true), m_kUpAxis(NiPoint3::UNIT_Z),
    m_bPromptOnDiscard(true), m_bHighlightSelected(true), m_bShowABVs(false),
    m_eCameraControlMode(CCMODE_MAYA)
{
}
//---------------------------------------------------------------------------
inline CNifUserPreferences::~CNifUserPreferences()
{ 
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetSaveOnExit(bool bSave)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    ms_bSaveOnExit = bSave;
}
//---------------------------------------------------------------------------
inline bool CNifUserPreferences::GetSaveOnExit()
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return  ms_bSaveOnExit;
}
//---------------------------------------------------------------------------
inline NiPoint3 CNifUserPreferences::GetTranslateSpeed() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return  m_kTranslateSpeed;
}
//---------------------------------------------------------------------------
inline float CNifUserPreferences::GetYawSpeed() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_fYawSpeed;
}
//---------------------------------------------------------------------------
inline float CNifUserPreferences::GetPitchSpeed() const
{ 
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_fPitchSpeed;
}
//---------------------------------------------------------------------------
inline CNifKeyboardShortcuts CNifUserPreferences::GetKeyboardShortcuts() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_kKeyboardShortcuts;
}
//---------------------------------------------------------------------------
inline NiColor CNifUserPreferences::GetBackgroundColor() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_kBackgroundColor;
}
//---------------------------------------------------------------------------
inline bool CNifUserPreferences::GetUseDefaultLights() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_bUseDefaultLights;
}
//---------------------------------------------------------------------------
inline float CNifUserPreferences::GetAnimationSliderSecondsPerTick() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_fAnimSecondsPerTick;
}
//---------------------------------------------------------------------------
inline NiPoint3 CNifUserPreferences::GetUpAxis() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);
    return m_kUpAxis;
}
//---------------------------------------------------------------------------
inline bool CNifUserPreferences::GetPromptOnDiscard() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_bPromptOnDiscard;
}
//---------------------------------------------------------------------------
inline bool CNifUserPreferences::GetHighlightSelected() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_bHighlightSelected;
}
//---------------------------------------------------------------------------
inline bool CNifUserPreferences::GetShowABVs() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_bShowABVs;
}
//---------------------------------------------------------------------------
inline CString CNifUserPreferences::GetCollisionLabPath()
{
    return m_strCollisionLabPath;
}
//---------------------------------------------------------------------------
inline CString CNifUserPreferences::GetImageSubfolderPath()
{
    return m_strImageSubfolderPath;
}
//---------------------------------------------------------------------------
inline CNifUserPreferences::CameraControlMode
    CNifUserPreferences::GetCameraControlMode() const
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    return m_eCameraControlMode;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetTranslateSpeed(const NiPoint3& kSpeed)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);  

    m_kTranslateSpeed = kSpeed;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetYawSpeed(float fSpeed)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);  

    m_fYawSpeed = fSpeed;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetPitchSpeed(float fSpeed)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_fPitchSpeed = fSpeed;
    ms_bBroadcastChanged = true;
}   
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetKeyboardShortcuts(
    CNifKeyboardShortcuts kShortcuts)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_kKeyboardShortcuts = kShortcuts;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetBackgroundColor(const NiColor& kColor)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_kBackgroundColor = kColor;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetUseDefaultLights(bool bUseDefaultLights)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_bUseDefaultLights = bUseDefaultLights;
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if (pkDoc)
    {
        pkDoc->EnableDefaultLights(m_bUseDefaultLights);
    }
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetAnimationSliderSecondsPerTick(
    float fTicksPerSec)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);
    ASSERT(fTicksPerSec >= 0);
    m_fAnimSecondsPerTick = fTicksPerSec;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetUpAxis(const NiPoint3& kUpAxis)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_kUpAxis = kUpAxis;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetPromptOnDiscard(bool bPromptOnDiscard)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_bPromptOnDiscard = bPromptOnDiscard;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetHighlightSelected(bool bHighlightSelected)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_bHighlightSelected = bHighlightSelected;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetShowABVs(bool bShowABVs)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_bShowABVs = bShowABVs;
    ms_bBroadcastChanged = true;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetCollisionLabPath(const char* pcPath)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_strCollisionLabPath = pcPath;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetImageSubfolderPath(const char* pcPath)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_strImageSubfolderPath = pcPath;
}
//---------------------------------------------------------------------------
inline void CNifUserPreferences::SetCameraControlMode(CameraControlMode eMode)
{
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    m_eCameraControlMode = eMode;
}