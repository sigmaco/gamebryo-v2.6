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
    public __value enum DefaultDock
    {
        Unknown,
        Float,
        DockTopAutoHide,
        DockLeftAutoHide,
        DockBottomAutoHide,
        DockRightAutoHide,
        Document,
        DockTop,
        DockLeft,
        DockBottom,
        DockRight,
        Hidden
    };

    /// <summary>
    /// Summary description for DockPositionAttribute.
    /// </summary>
    [AttributeUsage(AttributeTargets::Class, AllowMultiple=false,
        Inherited=false)]
    public __gc class DockPositionAttribute : public Attribute
    {
    public:
        DockPositionAttribute();

        __property DefaultDock get_Position();
        __property void set_Position(DefaultDock ePosition);
        __property bool get_AllowFloat();
        __property void set_AllowFloat(bool value);

    private:
        DefaultDock m_ePosition;
        bool m_bAllowFloat;
    };
}}}}
