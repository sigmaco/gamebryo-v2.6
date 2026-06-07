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
#include "IFilter.h"
#include "MScene.h"

using namespace System::Collections::Generic;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MFilterManager : public MDisposable
    {
    public:
        __property bool get_FilterEnabled();
        __property void set_FilterEnabled(bool bFilterEnabled);

        void AddFilter(IFilter* pmFilter);
        void RemoveFilter(IFilter* pmFilter);
        void ClearFilters();
        void UpdateFilter(IFilter* pmFilter);
        __property unsigned int get_FilterCount();
        IFilter* GetFilterAt(unsigned int uiIndex);

        MEntity* FilterEntities(MEntity* amInputEntities[])[];
        bool EntityMatchesFilter(MEntity* pmEntity);

    private:
        void OnNewSceneLoaded(MScene* pmScene);

        List<IFilter*>* m_pmFilters;
        bool m_bFilterEnabled;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MFilterManager* get_Instance();
    private:
        static MFilterManager* ms_pmThis = NULL;
        MFilterManager();
    };
}}}}
