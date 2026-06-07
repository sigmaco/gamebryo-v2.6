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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MSelectionSetService : public MDisposable,
        public ISelectionSetService
    {
    public:
        MSelectionSetService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // ISelectionSetService members.
    public:
        __property unsigned int get_SelectionSetCount();
        MSelectionSet* GetSelectionSets()[];
        MSelectionSet* GetSelectionSetByName(String* strName);
        bool AddSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveAllSelectionSets();
    };
}}}}
