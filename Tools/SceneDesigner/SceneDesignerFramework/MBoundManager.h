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

#include "MDisposable.h"
#include "ISettingsService.h"
#include "IOptionsService.h"

using namespace System::Collections;
using namespace System::Runtime::InteropServices;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MBoundManager : public MDisposable
    {
    private public:
        void Startup();

    public:
        void Update(float fTime);

        void OnSettingsChanged(Object* pmSender, 
            SettingChangedEventArgs* pmEventArgs);

        NiBound* GetSceneBound();
        NiBound* GetToolSceneBound(MViewport* pmViewport);
    protected:
        NiBound* m_pkSceneBound;
        MViewport* m_amViewports[];

        float m_fOrbitDistance;
        unsigned int m_uiUpdatesBetweenRecalculation;
        unsigned int m_uiUpdateCounter;

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;
    private:
        NiTObjectSet<NiBound>* m_pkToolBounds;

        // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

        // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MBoundManager* get_Instance();
    private:
        static MBoundManager* ms_pmThis = NULL;
        MBoundManager();
    };
}}}}
