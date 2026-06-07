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

// NifUserPreferences.h

#ifndef NIFUSERPREFERENCES_H
#define NIFUSERPREFERENCES_H

#include <NiRefObject.h>
#include "NifKeyboardShortcuts.h"

NiSmartPointer(CNifUserPreferences);

class CNifUserPreferences : public NiRefObject
{
public:
    enum CameraControlMode
    {
        CCMODE_MAYA,
        CCMODE_3DSMAX,
        CCMODE_COUNT
    };

    public:
        static void Lock();
        static CNifUserPreferences* AccessUserPreferences();
        static void UnLock();
        
        static void Destroy();
        static void Save();
        static void Load();
        static void SetSaveOnExit(bool bSave);
        static bool GetSaveOnExit();
        
        // Accessor functions
        NiPoint3 GetTranslateSpeed() const;
        float GetYawSpeed() const;
        float GetPitchSpeed() const;
        CNifKeyboardShortcuts GetKeyboardShortcuts() const;
        NiColor GetBackgroundColor() const;
        bool GetUseDefaultLights() const;
        float GetAnimationSliderSecondsPerTick() const;
        NiPoint3 GetUpAxis() const;
        bool GetPromptOnDiscard() const;
        bool GetHighlightSelected() const;
        bool GetShowABVs() const;
        CString GetCollisionLabPath();
        CString GetImageSubfolderPath();
        CameraControlMode GetCameraControlMode() const;

        // Setter functions
        void SetTranslateSpeed(const NiPoint3& kSpeed);
        void SetYawSpeed(float fSpeed);
        void SetPitchSpeed(float fSpeed);
        void SetKeyboardShortcuts(CNifKeyboardShortcuts kShortcuts);
        void SetBackgroundColor(const NiColor& kColor);
        void SetUseDefaultLights(bool bUseDefaultLights);
        void SetAnimationSliderSecondsPerTick(float fTicksPerSec);
        void SetUpAxis(const NiPoint3& kUpAxis);
        void SetPromptOnDiscard(bool bPromptOnDiscard);
        void SetHighlightSelected(bool bHighlightSelected);
        void SetShowABVs(bool bShowABVs);
        void SetCollisionLabPath(const char* pcPath);
        void SetImageSubfolderPath(const char* pcPath);
        void SetCameraControlMode(CameraControlMode eMode);

    protected:
        CNifUserPreferences();
        ~CNifUserPreferences();
        static void BroadcastChanged();
        static bool ms_bBroadcastChanged;

        static bool ms_bSaveOnExit;
        static CCriticalSection ms_kCriticalSection;
        static unsigned long ms_ulThreadId;
        static CNifUserPreferencesPtr ms_spThis;
        static bool ms_bWorking;
        
        float m_fAnimSecondsPerTick;
        NiPoint3 m_kTranslateSpeed;
        float m_fYawSpeed;
        float m_fPitchSpeed;
        CNifKeyboardShortcuts m_kKeyboardShortcuts;
        NiColor m_kBackgroundColor;
        bool m_bUseDefaultLights;
        NiPoint3 m_kUpAxis;
        bool m_bPromptOnDiscard;
        bool m_bHighlightSelected;
        bool m_bShowABVs;
        CameraControlMode m_eCameraControlMode;

        CString m_strCollisionLabPath;
        CString m_strImageSubfolderPath;
};

#include "NifUserPreferences.inl"

#endif  // #ifndef NIFUSERPREFERENCES_H
