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
    public __gc class Message
    {
    public:
        Message();
        Message(String* strText, String* strDetails, Object* pmObject);

        // The main text of the message.
        String* m_strText;

        // More detailed message text.
        String* m_strDetails;

        // A reference to some object that is associated with this message.
        Object* m_pmObject;

        // A string representing the time of the message.
        String* m_strTime;
    };
}}}}
