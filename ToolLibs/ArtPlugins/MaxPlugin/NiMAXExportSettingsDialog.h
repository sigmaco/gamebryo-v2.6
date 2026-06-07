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
#ifndef NIMAXEXPORTSETTINGSDIALOG_H
#define NIMAXEXPORTSETTINGSDIALOG_H
#include <NiDialog.h>
#include "NiMAXOptions.h"

#define NUM_PLATFORMS_SUPPORTED 5
class NiMAXExportSettingsDialog : public NiDialog
{
    public:
        NiMAXExportSettingsDialog(NiModuleRef hInstance, 
            NiWindowRef hWndParent);
        ~NiMAXExportSettingsDialog();
        int DoModal();
        virtual void InitDialog();
        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, 
            long lParam);
        virtual void OnOK();
        virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
            int iWParamHigh, long lParam);
        bool IsChecked(int iResource);

    protected:
        void OnPlatform();
        void OnSkinPercentageEdit();
        void OnTextureExport();
        void OnEditExportScriptButton();
        void OnEditProcessScriptButton();
        void OnSelectedExportScript();
        void OnSelectedProcessScript();
        void OnSelectedMeshProfile();
        void OnReloadProfilesButton();
        void RefreshImageFormatOptions();

        void CheckTextureExportRadio(int iSelection);
        void CheckPixelLayoutRadio(int iSelection);

        NiString GetTextureExport();
        NiSystemDesc::RendererID GetPlatform();
        NiTexture::FormatPrefs::PixelLayout GetPixelLayout();
        void EnableImageFormatOptions(HWND hDlg, bool bEnable);

        bool m_bOptionsChanged;
        NiScriptInfoPtr m_spNewProcessScript;
        NiScriptInfoPtr m_spNewExportScript;
        char m_acSceneMeshProfile[NI_MAX_PATH];
        NiScriptInfoSetPtr m_spAvailableExportScripts;
        NiScriptInfoSetPtr m_spAvailableProcessScripts;

        enum PlatformDropDownIds
        {
            ID_DD_GENERIC=0,  // "Default value", preserved for legacy
                              // interpretation (was PC in Gb 2.1).
            ID_DD_DX9,
            ID_DD_D3D10,
            ID_DD_PS3,
            ID_DD_WII,
            ID_DD_XENON,      // XBox 360 value, preserved for legacy 
                              // interpretation (was XENON in Gb 2.1).
            NUM_DD_IDS
        };

        enum
        {
            DD_STRING_LENGTH = 16
        };

        static const char ms_aPlatforms[NUM_DD_IDS][DD_STRING_LENGTH];

};

#endif