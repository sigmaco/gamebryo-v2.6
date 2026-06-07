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

using namespace System;

namespace NiManagedToolInterface
{
    // Delegates
    public __delegate void UpdateScrollEvent();

    public __gc class MLogger
    {
    public:
        static void LogDebug(System::String* pMessage);
        static void LogGeneral(System::String* pMessage);
        static void LogWarning(System::String* pMessage);
        static void LogEvent(System::String* pMessage);

        static void LogDebug(System::String* pMessage, bool bUpdateScroll);
        static void LogGeneral(System::String* pMessage, bool bUpdateScroll);
        static void LogWarning(System::String* pMessage, bool bUpdateScroll);
        static void LogEvent(System::String* pMessage, bool bUpdateScroll);

        static void Update();

        // Events
        static __event void add_OnUpdateScrollEvent(
            UpdateScrollEvent* pkEvent);
        static __event void remove_OnUpdateScrollEvent(
            UpdateScrollEvent* pkEvent);

    protected:
        // Delegates.
        static UpdateScrollEvent* m_pkUpdateScrollDelegate;

        // Events.
        static __event void raise_OnUpdateScrollEvent( );
    };
}
