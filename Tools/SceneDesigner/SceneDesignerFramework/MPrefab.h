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
#include "ICommandService.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;
using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MScene;
    public __gc class MEntity;

    public __gc class MPrefab : public MDisposable
    {
    public:
        MPrefab(MScene* pmScene);

        __property String* get_SourceFilename();  // full path name
        __property String* get_ShortName(); // filename w/out ext.
        __property String* get_Filename();  // filename including ext.
        __property String* get_Path();      // directory name
        __property MScene* get_Scene();
        __property bool get_IsInstantiated();

        __property bool get_Dirty();
        __property void set_Dirty(bool bDirty);

        __property bool get_Writable();


        MEntity* Instantiate()[];

        // Only called when loaded or substituted
        MEntity* CreateRoot(String* strPrefabName);

        bool Save();

    private:
        MScene* m_pmScene;

        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    };
}}}}
