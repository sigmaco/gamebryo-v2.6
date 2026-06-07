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

#include "StdAfx.h"
#include ".\mmessagemanager.h"
#include ".\mmessagechannel.h"

using namespace NiManagedToolInterface;

MMessageManager::MMessageManager(void)
{
    m_alMessageChannel = new ArrayList();

    // Hardcoded for right now....
    AddChannel("Debug");
    AddChannel("General");
    AddChannel("Warning");
    AddChannel("Events");
}

MMessageManager::~MMessageManager(void)
{
}
//---------------------------------------------------------------------------
int MMessageManager::AddChannel(String* strChannelName)
{
    int iNewMessageChannelIndex = m_alMessageChannel->Count;
    MMessageChannel* pkChannel =
        new MMessageChannel(strChannelName, iNewMessageChannelIndex );
    m_alMessageChannel->Add( pkChannel );

    raise_OnNewChannelEvent( pkChannel );

    return iNewMessageChannelIndex;
}
//---------------------------------------------------------------------------
MMessageChannel* MMessageManager::get_Channel(int iIndex)
{
    return static_cast<MMessageChannel*>(m_alMessageChannel->Item[iIndex]);
}
//---------------------------------------------------------------------------
ArrayList* MMessageManager::get_ChannelList()
{
    return m_alMessageChannel;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
void MMessageManager::add_OnNewChannelEvent(NewChannelEvent* pkEvent)
{
    m_pkNewChannelDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MMessageManager::remove_OnNewChannelEvent(NewChannelEvent* pkEvent)
{
    m_pkNewChannelDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MMessageManager::raise_OnNewChannelEvent( MMessageChannel* pkChannel )
{
    if (m_pkNewChannelDelegate)
    {
        m_pkNewChannelDelegate(pkChannel);
    }
}
//---------------------------------------------------------------------------


