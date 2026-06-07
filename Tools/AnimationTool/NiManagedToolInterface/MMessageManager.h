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

#include "MMessageChannel.h"

using namespace System::Collections;

namespace NiManagedToolInterface
{
    public __delegate void NewChannelEvent(MMessageChannel* pkChannel);

    __gc public class MMessageManager
    {
    public:
        MMessageManager(void);
        ~MMessageManager(void);

        int AddChannel(String* strChannelName);

        // Events
        __event void add_OnNewChannelEvent(NewChannelEvent* pkEvent);
        __event void remove_OnNewChannelEvent(NewChannelEvent* pkEvent);

        // Properties
        __property MMessageChannel* get_Channel(int iIndex);
        __property ArrayList* get_ChannelList();

    protected:

        // Events.
        __event void raise_OnNewChannelEvent( MMessageChannel* pkChannel );

        // Delegates.
        NewChannelEvent* m_pkNewChannelDelegate;

    private:
        // Currently only 1 
        ArrayList* m_alMessageChannel;
    };
}