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

#pragma once

#include "NiGridComponent.h"

using namespace System::Drawing;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
    {
    public __gc class MGridManager : public MDisposable
    {
    public:
        MGridManager();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
        void OnViewportCameraChanged(MViewport* pmViewport,
            MEntity* pmCamera);

        void CreateGridEntity();

    protected:
        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        float m_fSnapSpacing;
        int m_iSnapsPerLine;
        NiColor* m_pkColor;
        NiColor* m_pkMajorColor;
        int m_iExtent;
        int m_iMajorLines;
        bool m_bDisplayGrid;

        NiGridComponentPtr* m_pspGridComponents;
        unsigned int m_uiNumGridComponents;

    //inherited from MDisposable
    protected:
        void Do_Dispose(bool bDisposing);
    };
}}}}
