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
    public __gc class MRenderingModeService : public MDisposable,
        public IRenderingModeService
    {
    public:
        MRenderingModeService();

    private:
        ArrayList* m_pmModes;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // IRenderingModeService members.
    public:
        void AddRenderingMode(IRenderingMode* pmMode);
        IRenderingMode* GetRenderingModeByName(String* strName);
        IRenderingMode* GetRenderingModes()[];
    };
}}}}
