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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MMessageService : public MDisposable,
        public IMessageService
    {
    public:
        MMessageService();

    private:
        __gc struct Channel
        {
            Channel(String* strName) : m_strName(strName)
            {
                m_pmMessages = new ArrayList();
            }

            String* m_strName;
            ArrayList* m_pmMessages;
        };

        Channel* m_amChannels[];

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // IMessageService members.
    public:
        virtual __event MessageAddedHandler* MessageAdded;
        virtual __event ChannelMessagesClearedHandler* ChannelMessagesCleared;
        virtual __event AllMessagesClearedHandler* AllMessagesCleared;
        String* GetChannelName(MessageChannelType eChannel);
        void AddMessage(MessageChannelType eChannel, String* strMessage);
        void AddMessage(MessageChannelType eChannel, Message* pmMessage);
        Message* GetMessage(MessageChannelType eChannel, int iIndex);
        Message* GetLatestMessage(MessageChannelType eChannel);
        Message* GetMessages(MessageChannelType eChannel)[];
        void ClearMessages(MessageChannelType eChannel);
        void ClearAllMessages();
    };
}}}}
