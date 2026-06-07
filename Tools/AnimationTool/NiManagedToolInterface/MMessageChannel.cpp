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
#include "MMessageChannel.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MMessageChannel::MMessageChannel(int iThisChannel)
{
    m_iThisChannel = iThisChannel;
    m_bUpdateScroll = true;
}
//---------------------------------------------------------------------------
MMessageChannel::MMessageChannel(String* strName, int iThisChannel)
{
    m_iThisChannel = iThisChannel;
    m_strName = strName;
    m_alMessage = NiExternalNew ArrayList();
    m_alTime = NiExternalNew ArrayList();
    m_bUpdateScroll = true;
}
//---------------------------------------------------------------------------
MMessageChannel::~MMessageChannel(void)
{
    m_alMessage = NiExternalNew ArrayList();
    m_alTime = NiExternalNew ArrayList();
}
//---------------------------------------------------------------------------
String* MMessageChannel::get_Name()
{
    return m_strName;
}
//---------------------------------------------------------------------------
void MMessageChannel::set_Name(String* strName)
{
    m_strName = strName;

    // RaiseNameChangeEvent
}
//---------------------------------------------------------------------------
void MMessageChannel::Add(String* strMessage)
{
    Add(strMessage, true);
}
//---------------------------------------------------------------------------
void MMessageChannel::Add(String* strMessage, bool bUpdateScroll)
{
    m_alMessage->Add( strMessage );

    float fCurrentTime = MUtility::GetCurrentTimeInSec();
    String* strFloat = System::Convert::ToString(fCurrentTime);
    m_alTime->Add( strFloat );
    m_bUpdateScroll = bUpdateScroll;

    // Added Message Event Raised
    raise_OnNewMessageEvent( this );
}
//---------------------------------------------------------------------------
String* MMessageChannel::get_Message(int iIndex)
{
    return static_cast<String*>(m_alMessage->Item[iIndex]);
}
//---------------------------------------------------------------------------
void MMessageChannel::set_Message(int iIndex, String* strMessage)
{
    m_alMessage->Item[iIndex] = strMessage;

    // Changed Message Event Raised
}
//---------------------------------------------------------------------------
String* MMessageChannel::get_LatestMessage()
{
    int iIndex = m_alMessage->Count - 1;
    return static_cast<String*>(m_alMessage->Item[iIndex]);
}
//---------------------------------------------------------------------------
ArrayList* MMessageChannel::get_MessageList()
{
    return m_alMessage;
}
//---------------------------------------------------------------------------
int MMessageChannel::get_ChannelID()
{
    return this->m_iThisChannel;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
String* MMessageChannel::get_Time(int iIndex)
{
    return static_cast<String*>(m_alTime->Item[iIndex]);
}
//---------------------------------------------------------------------------
String* MMessageChannel::get_LatestTime()
{
    int iIndex = m_alTime->Count - 1;
    return static_cast<String*>(m_alTime->Item[iIndex]);
}
//---------------------------------------------------------------------------
ArrayList* MMessageChannel::get_TimeList()
{
    return m_alTime;
}
//---------------------------------------------------------------------------
bool MMessageChannel::get_UpdateScroll()
{
    return m_bUpdateScroll;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
void MMessageChannel::add_OnNewMessageEvent(NewMessageEvent* pkEvent)
{
    m_pkNewMessageDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MMessageChannel::remove_OnNewMessageEvent(NewMessageEvent* pkEvent)
{
    m_pkNewMessageDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MMessageChannel::raise_OnNewMessageEvent( MMessageChannel* pkChannel )
{
    if (m_pkNewMessageDelegate)
    {
        m_pkNewMessageDelegate(pkChannel);
    }
}
//---------------------------------------------------------------------------



