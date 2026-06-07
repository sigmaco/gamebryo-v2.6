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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MMessageService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MMessageService::MMessageService()
{
    String* astrNames[] = Enum::GetNames(__typeof(MessageChannelType));
    int iLength = astrNames->Length;

    m_amChannels = new Channel*[iLength];
    for (int i = 0; i < iLength; i++)
    {
        m_amChannels[i] = new Channel(astrNames[i]);
    }
}
//---------------------------------------------------------------------------
void MMessageService::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        m_amChannels = NULL;
    }
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MMessageService::get_Name()
{
    MVerifyValidInstance;

    return "Message Service";
}
//---------------------------------------------------------------------------
bool MMessageService::Initialize()
{
    MVerifyValidInstance;
    
    return true;
}
//---------------------------------------------------------------------------
bool MMessageService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IMessageService members.
//---------------------------------------------------------------------------
String* MMessageService::GetChannelName(MessageChannelType eChannel)
{
    MVerifyValidInstance;

    return m_amChannels[eChannel]->m_strName;
}
//---------------------------------------------------------------------------
void MMessageService::AddMessage(MessageChannelType eChannel,
    String* strMessage)
{
    MVerifyValidInstance;

    AddMessage(eChannel, new Message(strMessage, NULL, NULL));
}
//---------------------------------------------------------------------------
void MMessageService::AddMessage(MessageChannelType eChannel,
    Message* pmMessage)
{
    MVerifyValidInstance;

    pmMessage->m_strTime = DateTime::Now.ToLongTimeString();
    int iIndex = m_amChannels[eChannel]->m_pmMessages->Add(pmMessage);
    MessageAdded(eChannel, iIndex);
}
//---------------------------------------------------------------------------
Message* MMessageService::GetMessage(MessageChannelType eChannel, int iIndex)
{
    MVerifyValidInstance;

    if (iIndex < m_amChannels[eChannel]->m_pmMessages->Count)
    {
        return dynamic_cast<Message*>(
            m_amChannels[eChannel]->m_pmMessages->Item[iIndex]);
    }

    return NULL;
}
//---------------------------------------------------------------------------
Message* MMessageService::GetLatestMessage(MessageChannelType eChannel)
{
    MVerifyValidInstance;

    return dynamic_cast<Message*>(m_amChannels[eChannel]->m_pmMessages->Item[
        m_amChannels[eChannel]->m_pmMessages->Count - 1]);
}
//---------------------------------------------------------------------------
Message* MMessageService::GetMessages(MessageChannelType eChannel)[]
{
    MVerifyValidInstance;

    return dynamic_cast<Message*[]>(
        m_amChannels[eChannel]->m_pmMessages->ToArray(__typeof(Message)));
}
//---------------------------------------------------------------------------
void MMessageService::ClearMessages(MessageChannelType eChannel)
{
    MVerifyValidInstance;

    m_amChannels[eChannel]->m_pmMessages->Clear();
    ChannelMessagesCleared(eChannel);
}
//---------------------------------------------------------------------------
void MMessageService::ClearAllMessages()
{
    MVerifyValidInstance;

    for (int i = 0; i < m_amChannels->Length; i++)
    {
        m_amChannels[i]->m_pmMessages->Clear();
    }
    AllMessagesCleared();
}
//---------------------------------------------------------------------------
