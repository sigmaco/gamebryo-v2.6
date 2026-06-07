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

#include "stdafx.h"
#include "MCompletionInfo.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MCompletionInfo::MCompletionInfo() : m_bDisposed(false)
{
    m_pkCompletionInfo = NiNew NiActorManager::CompletionInfo;
}
//---------------------------------------------------------------------------
MCompletionInfo::~MCompletionInfo()
{
    Dispose(true);
}
//---------------------------------------------------------------------------
void MCompletionInfo::Dispose()
{
    Dispose(false);
}
//---------------------------------------------------------------------------
void MCompletionInfo::Dispose(bool bFromDestructor)
{
    __try
    {
        Threading::Monitor::Enter(this);
        if (!m_bDisposed)
        {
            if (m_pkCompletionInfo)
            {
                NiDelete m_pkCompletionInfo;
                m_pkCompletionInfo = NULL;

                if (!bFromDestructor)
                {
                    GC::SuppressFinalize(this);
                }
            }
            m_bDisposed = true;
        }
    }
    __finally
    {
        Threading::Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
NiActorManager::CompletionInfo* MCompletionInfo::GetCompletionInfo()
{
    return m_pkCompletionInfo;
}
//---------------------------------------------------------------------------
void MCompletionInfo::BuildChainCompletionInfoArray()
{
    m_aChainCompletionInfo = NULL;

    unsigned int uiCount = m_pkCompletionInfo->m_kChainCompletionInfoSet
        .GetSize();
    if (uiCount > 0)
    {
        m_aChainCompletionInfo = NiExternalNew MChainCompletionInfo*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            m_aChainCompletionInfo[ui] = NiExternalNew MChainCompletionInfo(
                m_pkCompletionInfo->m_kChainCompletionInfoSet.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
float MCompletionInfo::get_FrameInDestWhenTransitionCompletes()
{
    return m_pkCompletionInfo->m_fFrameInDestWhenTransitionCompletes;
}
//---------------------------------------------------------------------------
float MCompletionInfo::get_TimeToCompleteTransition()
{
    return m_pkCompletionInfo->m_fTimeToCompleteTransition;
}
//---------------------------------------------------------------------------
float MCompletionInfo::get_FrameTransitionOccursInSrc()
{
    return m_pkCompletionInfo->m_fFrameTransitionOccursInSrc;
}
//---------------------------------------------------------------------------
float MCompletionInfo::get_TimeForChainToComplete()
{
    return m_pkCompletionInfo->m_fTimeForChainToComplete;
}
//---------------------------------------------------------------------------
MTransition::MBlendPair* MCompletionInfo::get_BlendPair()
{
    return NiExternalNew MTransition::MBlendPair(
        m_pkCompletionInfo->m_pkBlendPair);
}
//---------------------------------------------------------------------------
MChainCompletionInfo* MCompletionInfo::get_ChainCompletionInfo()[]
{
    return m_aChainCompletionInfo;
}
//---------------------------------------------------------------------------
MChainCompletionInfo::MChainCompletionInfo(
    NiActorManager::ChainCompletionInfo* pkChainCompletionInfo) :
    m_pkChainCompletionInfo(NULL)
{
    NIASSERT(pkChainCompletionInfo != NULL);
    SetChainCompletionInfo(pkChainCompletionInfo);
}
//---------------------------------------------------------------------------
MChainCompletionInfo::~MChainCompletionInfo()
{
    Dispose(true);
}
//---------------------------------------------------------------------------
void MChainCompletionInfo::Dispose()
{
    Dispose(false);
}
//---------------------------------------------------------------------------
void MChainCompletionInfo::Dispose(bool bFromDestructor)
{
    __try
    {
        Threading::Monitor::Enter(this);
        if (!m_bDisposed)
        {
            if (m_pkChainCompletionInfo)
            {
                m_pkChainCompletionInfo = NULL;

                if (!bFromDestructor)
                {
                    GC::SuppressFinalize(this);
                }
            }
            m_bDisposed = true;
        }
    }
    __finally
    {
        Threading::Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
void MChainCompletionInfo::SetChainCompletionInfo(
    NiActorManager::ChainCompletionInfo* pkChainCompletionInfo)
{
    m_pkChainCompletionInfo = pkChainCompletionInfo;
}
//---------------------------------------------------------------------------
float MChainCompletionInfo::get_SeqStart()
{
    return m_pkChainCompletionInfo->m_fSeqStart;
}
//---------------------------------------------------------------------------
float MChainCompletionInfo::get_TransStart()
{
    return m_pkChainCompletionInfo->m_fTransStart;
}
//---------------------------------------------------------------------------
float MChainCompletionInfo::get_TransEnd()
{
    return m_pkChainCompletionInfo->m_fTransEnd;
}
//---------------------------------------------------------------------------
float MChainCompletionInfo::get_InSeqBeginFrame()
{
    return m_pkChainCompletionInfo->m_fInSeqBeginFrame;
}
//---------------------------------------------------------------------------
String* MChainCompletionInfo::get_Name()
{
    return (const char*)m_pkChainCompletionInfo->m_kName;
}
//---------------------------------------------------------------------------
String* MChainCompletionInfo::get_NextName()
{
    return (const char*)m_pkChainCompletionInfo->m_kNextName;
}
//---------------------------------------------------------------------------
MTransition::TransitionType MChainCompletionInfo::get_TransitionType()
{
    return MTransition::TranslateTransitionType(
        m_pkChainCompletionInfo->m_pkTransition->GetType(),
        NiExternalNew MTransition(MFramework::Instance->Animation,
        m_pkChainCompletionInfo->m_uiSrcID,
        m_pkChainCompletionInfo->m_uiDesID,
        m_pkChainCompletionInfo->m_pkTransition));
}
//---------------------------------------------------------------------------
