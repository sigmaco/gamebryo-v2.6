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
    /// Indicates that the method to which this attribute is
    /// applied handles the command named in the constructor
    /// </summary>
    /// <remarks>
    /// The handler method must must match the signature required 
    /// by the System.EventHandler Delegate.
    /// <b>Requirements</b>
    /// For static methods that are marked with this attribute,
    /// The Command manager plug-in will bind these methods automatically
    /// so long as the Assembly they are in is marked with the
    /// "ContainsUICommands"
    /// 
    /// For instance methods, the class must call 
    /// ICommandService.BindCommands and pass itself to have all
    /// non static command handlers registered.
    /// </remarks>
    [Flags]
    public __value enum Interactive
    {
        None,
        Unspecified     = 0x1,
        NonInteractive  = 0x2,
        Interactive     = 0x4
    };

    [AttributeUsage(AttributeTargets::Method, AllowMultiple=true)]
    public __gc class UICommandHandlerAttribute : public Attribute
    {
    public:
        UICommandHandlerAttribute(String* strName);

        __property String* get_Name();
        __property void set_Name(String* strName);

        //Emergent Internal Use only
        __property Interactive get_Interactivity();
        __property void set_Interactivity(Interactive eValue);
        //End emergent internal use

    private:
        String* m_strName;
        Interactive m_eInteractive;
        bool m_bInherited;
    };
}}}}
