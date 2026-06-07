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
    /// applied handles deciding whether the command is currently 
    /// a valid operation
    /// </summary>
    /// <remarks>
    /// The handler method must must match the following
    /// signature:
    /// 'bool SomeFunction()'
    /// <b>Requirements</b>
    /// For static methods that are marked with this attribute,
    /// The Command manager plug-in will bind these methods automatically
    /// so long as the Assembly they are in is marked with the
    /// "ContainsUICommands"
    /// 
    /// For instance methods, the class must call 
    /// ICommandService.BindCommands and pass itself to have all
    /// non static command validators registered.
    /// </remarks>
    [AttributeUsage(AttributeTargets::Method, AllowMultiple=false)]
    public __gc __sealed class UICommandValidatorAttribute : public Attribute
    {
    public:
        UICommandValidatorAttribute(String* strName);

        __property String* get_Name();
        __property void set_Name(String* strName);

    private:
        String* m_strName;
    };
}}}}
