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
    /// applied is a UICommand Handler that requires the named parameter
    /// </summary>
    /// <remarks>
    /// The UICommand system will check that an invoker of this 
    /// command is passing the named parameters to the UICommand
    /// 
    /// </remarks>
    [AttributeUsage(AttributeTargets::Method, AllowMultiple=true)]
    public __gc class RequiresParameterAttribute : public Attribute
    {
    public:
        RequiresParameterAttribute(String* strName);

        __property String* get_Name();
        __property void set_Name(String* strName);

    private:
        String* m_strName;
    };
}}}}
