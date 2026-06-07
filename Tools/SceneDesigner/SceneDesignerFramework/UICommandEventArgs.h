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
    namespace PluginAPI
{
    public __gc class UICommandEventArgs : public EventArgs
    {
    public:
        UICommandEventArgs();
        String* GetParameters()[];
        String* GetValue(String* strParameterName);
        void SetValue(String* strParameterName, String* strValue);
    private:
        Hashtable* m_pmParameters;
    };

}}}}
