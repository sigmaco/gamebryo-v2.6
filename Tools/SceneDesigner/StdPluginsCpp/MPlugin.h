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

#include "MGridManager.h"
#include "MSelectionService.h"
#include "MToolbarListener.h"
#include "MSettingsHelper.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MPlugin : public MDisposable, public IPlugin
    {
    public:
        MPlugin();

        __property String* get_Name();
        __property System::Version* get_Version();
        __property System::Version* get_ExpectedVersion();
        void Load(int iToolMajorVersion, int iToolMinorVersion);
        IService* GetProvidedServices()[];
        void Start();

    private:
        MGridManager* m_pmGridManager;
        MSelectionService* m_pmSelectionService;
        MToolbarListener* m_pmToolbarListener;

        void AddPropertyTypes();
        void RegisterComponents();
        void RegisterSettings();
        void AddInteractionModes();
        void AddRenderingModes();
        void AddGrid();
        void AddToolbarListener();
        [DllInit]
        static void InitStatics();

        [DllShutdown]
        static void ShutdownStatics();

    // MDisposable member
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
