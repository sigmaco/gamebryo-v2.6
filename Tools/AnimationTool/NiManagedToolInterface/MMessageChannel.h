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

namespace NiManagedToolInterface
{
    __gc public class MMessageChannel;

    // Delegates
    public __delegate void NewMessageEvent(MMessageChannel* pkChannel);

    __gc public class MMessageChannel
    {
    public:
        MMessageChannel(int iThisChannel);
        MMessageChannel(String* strName, int iThisChannel);
        ~MMessageChannel(void);

        void Add(String* strMessage);
        void Add(String* strMessage, bool bUpdateScroll);

        // Events
        __event void add_OnNewMessageEvent(NewMessageEvent* pkEvent);
        __event void remove_OnNewMessageEvent(NewMessageEvent* pkEvent);

        // Properties
        __property String* get_Name();
        __property void set_Name(String* strName);
        __property String* get_Message(int iIndex);
        __property void set_Message(int iIndex, String* strMessage);
        __property String* get_LatestMessage(); 
        __property ArrayList* get_MessageList();
        __property int get_ChannelID();
        __property String* get_Time(int iIndex);
        __property String* get_LatestTime();
        __property ArrayList* get_TimeList();
        __property bool get_UpdateScroll();
    
    protected:
        // Events.
        __event void raise_OnNewMessageEvent( MMessageChannel* pkChannel );

        // Delegates.
        NewMessageEvent* m_pkNewMessageDelegate;

    private:
        int m_iThisChannel;
        String* m_strName;
        ArrayList* m_alMessage; // Array of Strings
        ArrayList* m_alTime; // Array of Strings 
        bool m_bUpdateScroll; // Indicates this channel is activel scrolling
    };
}