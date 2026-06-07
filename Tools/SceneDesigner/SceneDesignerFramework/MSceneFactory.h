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

using namespace System::Collections;

#include "MDisposable.h"
#include "MScene.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MSceneFactory : public MDisposable
    {
    public:
        MScene* Get(NiScene* pkUnmanaged);
        void Remove(NiScene* pkUnmanaged);
        void Remove(MScene* pmManaged);
        void Clear();

    private:
        Hashtable* m_pmUnmanagedToManaged;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MSceneFactory* get_Instance();
    private:
        static MSceneFactory* ms_pmThis = NULL;
        MSceneFactory();
    };
}}}}
