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
    namespace PluginAPI
{
    /// <summary>
    /// Summary description for UIState.
    /// </summary>
    public __gc class UIState
    {
    public:
        UIState();

        __property bool get_Enabled();
        __property void set_Enabled(bool bEnabled);

        __property bool get_Checked();
        __property void set_Checked(bool bChecked);

        __property String* get_Text();
        __property void set_Text(String* strText);

    private:
        bool m_bEnabled;
        bool m_bChecked;
        String* m_strText;
    };
}}}}
