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

// NifStatisticsManager.cpp

#include "stdafx.h"
#include "NifStatisticsManager.h"
#include "NifDoc.h"

CCriticalSection CNifStatisticsManager::ms_kCriticalSection;
unsigned long CNifStatisticsManager::ms_ulThreadId = 0;
CNifStatisticsManagerPtr CNifStatisticsManager::ms_spThis = 0;
bool CNifStatisticsManager::ms_bWorking = false;

//---------------------------------------------------------------------------
void CNifStatisticsManager::Lock()
{ 
    ms_bWorking = ms_kCriticalSection.Lock()? true:false;
    ms_ulThreadId = GetCurrentThreadId();
}
//---------------------------------------------------------------------------
CNifStatisticsManager* CNifStatisticsManager::AccessStatisticsManager()
{ 
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    if(!ms_spThis)
    {
        ms_spThis = NiNew CNifStatisticsManager();
    }
    return ms_spThis;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::UnLock()
{ 
    ASSERT(ms_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == ms_ulThreadId);

    ms_bWorking = ms_kCriticalSection.Unlock() ? false : true;
    if(ms_kCriticalSection.m_sect.LockCount == -1)
        ms_ulThreadId = NULL;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::Destroy()
{
    Lock();
    ms_spThis = 0;
    UnLock();
}
//---------------------------------------------------------------------------
CNifStatisticsManager::CNifStatisticsManager()
{
    ResetTimers();
    m_fFrameRateSampleTime = 1.0f;
}
//---------------------------------------------------------------------------
CNifStatisticsManager::~CNifStatisticsManager()
{
    ResetRoots();
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::ResetTimers()
{
    ResetFrameRateTimer();

    m_fFrameRate = 0.0f;

    m_fUpdateTimer = -1.0f;
    m_fSwapTimer = -1.0f;

    m_fUpdateTime = 0.0f;
    m_fCullTime = 0.0f;
    m_fRenderTime = 0.0f;
    m_fSwapTime = 0.0f;

    m_uiLastNumObjectsDrawnPerFrame = 0;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::SetNumObjectsDrawnPerFrame(
    unsigned int uiNumObjs)
{
    m_uiLastNumObjectsDrawnPerFrame = uiNumObjs;
}
//---------------------------------------------------------------------------
unsigned int CNifStatisticsManager::GetNumObjectsDrawnPerFrame()
{
    return m_uiLastNumObjectsDrawnPerFrame;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::ResetFrameRateTimer()
{
    m_uiNumSamples = 0;
    m_fTimeElapsed = 0.0f;
    m_fLastTime = -1.0f;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::SetFrameRateSampleTime(float fSampleTime)
{
    if (fSampleTime > 0.0f)
    {
        m_fFrameRateSampleTime = fSampleTime;
    }
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetFrameRateSampleTime() const
{
    return m_fFrameRateSampleTime;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::SampleFrameRate()
{
    float fCurrentTime = NiGetCurrentTimeInSec();
    if (m_fLastTime == -1.0f)
    {
        m_fLastTime = fCurrentTime;
    }
    float fDelta = fCurrentTime - m_fLastTime;
    m_fLastTime = fCurrentTime;
    m_uiNumSamples++;
    m_fTimeElapsed += fDelta;

    if (m_fTimeElapsed > m_fFrameRateSampleTime)
    {
        m_fFrameRate = ((float) m_uiNumSamples) / m_fTimeElapsed;
        ResetFrameRateTimer();
    }
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetFrameRate()
{
    return m_fFrameRate;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::StartUpdateTimer()
{
    if (m_fUpdateTimer == -1.0f)
    {
        m_fUpdateTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::StopUpdateTimer()
{
    if (m_fUpdateTimer != -1.0f)
    {
        m_fUpdateTime = NiGetCurrentTimeInSec() - m_fUpdateTimer;
        m_fUpdateTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetUpdateTime()
{
    return m_fUpdateTime;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::SetCullTime(float fCullTime)
{
    m_fCullTime = fCullTime;
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetCullTime()
{
    return m_fCullTime;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::SetRenderTime(float fRenderTime)
{
    m_fRenderTime = fRenderTime;
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetRenderTime()
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::StartSwapTimer()
{
    if (m_fSwapTimer == -1.0f)
    {
        m_fSwapTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::StopSwapTimer()
{
    if (m_fSwapTimer != -1.0f)
    {
        m_fSwapTime = NiGetCurrentTimeInSec() - m_fSwapTimer;
        m_fSwapTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
float CNifStatisticsManager::GetSwapTime()
{
    return m_fSwapTime;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::ResetRoots()
{
    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        RootInfo* pkInfo = m_kRoots.GetAt(ui);
        delete pkInfo;
    }
    m_kRoots.RemoveAll();
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::AddRoot(NiNode* pkRoot)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if (pkDoc)
    {
        RootInfo* pkInfo = NiNew RootInfo;
        pkInfo->m_pkRoot = pkRoot;
        pkInfo->m_uiNumObjects = 0;
        pkInfo->m_uiNumTriangles = 0;
        pkInfo->m_uiNumVertices = 0;

        pkDoc->Lock();
        GatherStats(pkRoot, pkInfo);
        pkDoc->UnLock();

        m_kRoots.Add(pkInfo);
    }
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::RemoveRoot(NiNode* pkRoot)
{
    unsigned int uiIndex;
    for (uiIndex = 0; uiIndex < m_kRoots.GetSize(); uiIndex++)
    {
        if (m_kRoots.GetAt(uiIndex)->m_pkRoot == pkRoot)
        {
            break;
        }
    }
    RootInfo* pkInfo = m_kRoots.GetAt(uiIndex);
    delete pkInfo;
    m_kRoots.RemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int CNifStatisticsManager::GetNumObjects()
{
    unsigned int uiNumObjects = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumObjects += m_kRoots.GetAt(ui)->m_uiNumObjects;
    }

    return uiNumObjects;
}
//---------------------------------------------------------------------------
unsigned int CNifStatisticsManager::GetNumTriangles()
{
    unsigned int uiNumTriangles = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumTriangles += m_kRoots.GetAt(ui)->m_uiNumTriangles;
    }

    return uiNumTriangles;
}
//---------------------------------------------------------------------------
unsigned int CNifStatisticsManager::GetNumVertices()
{
    unsigned int uiNumVertices = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumVertices += m_kRoots.GetAt(ui)->m_uiNumVertices;
    }

    return uiNumVertices;
}
//---------------------------------------------------------------------------
void CNifStatisticsManager::GatherStats(NiAVObject* pkObject,
    RootInfo* pkInfo)
{  
    pkInfo->m_uiNumObjects++;
    if (NiIsKindOf(NiMesh, pkObject))
    {   
        pkInfo->m_uiNumVertices += ((NiMesh*) pkObject)->GetVertexCount();
        pkInfo->m_uiNumTriangles +=
            ((NiMesh*) pkObject)->GetTotalPrimitiveCount();
    }
    
    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                GatherStats(pkChild, pkInfo);
            }
        }
    }
}
//---------------------------------------------------------------------------
