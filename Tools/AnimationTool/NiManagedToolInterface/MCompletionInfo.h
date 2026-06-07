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

#include "MTransition.h"

namespace NiManagedToolInterface
{
    public __gc class MChainCompletionInfo : public IDisposable
    {
    public:
        __property float get_SeqStart();
        __property float get_TransStart();
        __property float get_TransEnd();
        __property float get_InSeqBeginFrame();
        __property String* get_Name();
        __property String* get_NextName();
        __property MTransition::TransitionType get_TransitionType();

        MChainCompletionInfo(NiActorManager::ChainCompletionInfo*
            pkChainCompletionInfo);
        ~MChainCompletionInfo();

    private:
        void SetChainCompletionInfo(NiActorManager::ChainCompletionInfo*
            pkChainCompletionInfo);

        void Dispose(bool bFromDestructor);
        bool m_bDisposed;

        NiActorManager::ChainCompletionInfo* m_pkChainCompletionInfo;

    // IDisposable members.
    public:
        void Dispose();
    };

    public __gc class MCompletionInfo : public IDisposable
    {
    public:
        __property float get_FrameInDestWhenTransitionCompletes();
        __property float get_TimeToCompleteTransition();
        __property float get_FrameTransitionOccursInSrc();
        __property float get_TimeForChainToComplete();
        __property MTransition::MBlendPair* get_BlendPair();
        __property MChainCompletionInfo* get_ChainCompletionInfo()[];

        MCompletionInfo();
        ~MCompletionInfo();
        NiActorManager::CompletionInfo* GetCompletionInfo();
        void BuildChainCompletionInfoArray();

    private:
        void Dispose(bool bFromDestructor);
        bool m_bDisposed;

        NiActorManager::CompletionInfo* m_pkCompletionInfo;
        MChainCompletionInfo* m_aChainCompletionInfo[];

    // IDisposable members.
    public:
        void Dispose();
    };
}
