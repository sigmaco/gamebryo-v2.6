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
#include "MStatisticsManager.h"
#include <NiMain.h>

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MStatisticsManager::MStatisticsManager()
{
    ResetTimers();
    m_fFrameRateSampleTime = 1.0f;
}
//---------------------------------------------------------------------------
MStatisticsManager::~MStatisticsManager()
{
    ResetRoots();
}
//---------------------------------------------------------------------------
void MStatisticsManager::ResetTimers()
{
    ResetFrameRateTimer();

    m_fFrameRate = 0.0f;

    m_fUpdateTimer = -1.0f;
    m_fClearTimer = -1.0f;
    m_fClickTimer = -1.0f;
    m_fSwapTimer = -1.0f;

    m_fLastUpdateTime = 0.0f;
    m_fLastClearTime = 0.0f;
    m_fLastClickTime = 0.0f;
    m_fLastSwapTime = 0.0f;
}
//---------------------------------------------------------------------------
void MStatisticsManager::ResetFrameRateTimer()
{
    m_uiNumSamples = 0;
    m_fTimeElapsed = 0.0f;
    m_fLastTime = -1.0f;
}
//---------------------------------------------------------------------------
void MStatisticsManager::SetFrameRateSampleTime(float fSampleTime)
{
    if (fSampleTime > 0.0f)
    {
        m_fFrameRateSampleTime = fSampleTime;
    }
}
//---------------------------------------------------------------------------
float MStatisticsManager::GetFrameRateSampleTime() const
{
    return m_fFrameRateSampleTime;
}
//---------------------------------------------------------------------------
void MStatisticsManager::SampleFrameRate()
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
float MStatisticsManager::GetFrameRate()
{
    return m_fFrameRate;
}
//---------------------------------------------------------------------------
void MStatisticsManager::StartUpdateTimer()
{
    if (m_fUpdateTimer == -1.0f)
    {
        m_fUpdateTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StartClearTimer()
{
    if (m_fClearTimer == -1.0f)
    {
        m_fClearTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StartClickTimer()
{
    if (m_fClickTimer == -1.0f)
    {
        m_fClickTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StartSwapTimer()
{
    if (m_fSwapTimer == -1.0f)
    {
        m_fSwapTimer = NiGetCurrentTimeInSec();
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StopUpdateTimer()
{
    if (m_fUpdateTimer != -1.0f)
    {
        m_fLastUpdateTime = NiGetCurrentTimeInSec() - m_fUpdateTimer;
        m_fUpdateTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StopClearTimer()
{
    if (m_fClearTimer != -1.0f)
    {
        m_fLastClearTime = NiGetCurrentTimeInSec() - m_fClearTimer;
        m_fClearTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StopClickTimer()
{
    if (m_fClickTimer != -1.0f)
    {
        m_fLastClickTime = NiGetCurrentTimeInSec() - m_fClickTimer;
        m_fClickTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
void MStatisticsManager::StopSwapTimer()
{
    if (m_fSwapTimer != -1.0f)
    {
        m_fLastSwapTime = NiGetCurrentTimeInSec() - m_fSwapTimer;
        m_fSwapTimer = -1.0f;
    }
}
//---------------------------------------------------------------------------
float MStatisticsManager::GetLastUpdateTime()
{
    return m_fLastUpdateTime;
}
//---------------------------------------------------------------------------
float MStatisticsManager::GetLastClearTime()
{
    return m_fLastClearTime;
}
//---------------------------------------------------------------------------
float MStatisticsManager::GetLastClickTime()
{
    return m_fLastClickTime;
}
//---------------------------------------------------------------------------
float MStatisticsManager::GetLastSwapTime()
{
    return m_fLastSwapTime;
}
//---------------------------------------------------------------------------
void MStatisticsManager::ResetRoots()
{
    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        RootInfo* pkInfo = m_kRoots.GetAt(ui);
        NiExternalDelete pkInfo;
    }
    m_kRoots.RemoveAll();
}
//---------------------------------------------------------------------------
void MStatisticsManager::AddRoot(NiNode* pkRoot)
{
    RootInfo* pkInfo = NiExternalNew RootInfo;
    pkInfo->m_pkRoot = pkRoot;
    pkInfo->m_uiNumObjects = 0;
    pkInfo->m_uiNumTriangles = 0;
    pkInfo->m_uiNumVertices = 0;

    GatherStats(pkRoot, pkInfo);

    m_kRoots.Add(pkInfo);
}
//---------------------------------------------------------------------------
void MStatisticsManager::RemoveRoot(NiNode* pkRoot)
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
    NiExternalDelete pkInfo;
    m_kRoots.RemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int MStatisticsManager::GetNumObjects()
{
    unsigned int uiNumObjects = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumObjects += m_kRoots.GetAt(ui)->m_uiNumObjects;
    }

    return uiNumObjects;
}
//---------------------------------------------------------------------------
unsigned int MStatisticsManager::GetNumTriangles()
{
    unsigned int uiNumTriangles = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumTriangles += m_kRoots.GetAt(ui)->m_uiNumTriangles;
    }

    return uiNumTriangles;
}
//---------------------------------------------------------------------------
unsigned int MStatisticsManager::GetNumVertices()
{
    unsigned int uiNumVertices = 0;

    for (unsigned int ui = 0; ui < m_kRoots.GetSize(); ui++)
    {
        uiNumVertices += m_kRoots.GetAt(ui)->m_uiNumVertices;
    }

    return uiNumVertices;
}
//---------------------------------------------------------------------------
void MStatisticsManager::GatherStats(NiAVObject* pkObject,
    RootInfo* pkInfo)
{
    pkInfo->m_uiNumObjects++;

    NIASSERT(NiIsKindOf(NiTriBasedGeom, pkObject) == false);
    if (NiIsKindOf(NiMesh, pkObject))
    {
        pkInfo->m_uiNumVertices += 
            ((NiMesh*)pkObject)->GetVertexCount();
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
