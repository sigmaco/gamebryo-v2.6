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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MComponentService : public MDisposable,
        public IComponentService
    {
    public:
        MComponentService();

    private:
        Hashtable* m_pmNameToComponent;
        Hashtable* m_pmIDToComponent;
        Hashtable* m_pmIDToVisible;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // IComponentService members.
    public:
        bool RegisterComponent(MComponent* pmComponent);
        bool RegisterComponent(MComponent* pmComponent, bool bVisibleToUser);
        void UnregisterComponent(MComponent* pmComponent);
        void UnregisterAllComponents();
        String* GetComponentNames()[];
        MComponent* GetAllComponents()[];
        MComponent* CloneComponentByID(Guid mGuid);
        MComponent* GetComponentByID(Guid mGuid);
        bool IsComponentVisibleToUser(MComponent* pmComponent);
    };
}}}}
