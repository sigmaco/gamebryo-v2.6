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

#include "NiExporterOptionsSharedData.h"
#include "NiScriptInfo.h"
NiImplementRTTI(NiExporterOptionsSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiExporterOptionsSharedData::NiExporterOptionsSharedData()
{
    m_spCurrentScript = NULL;
    m_bViewerRun = false;
    m_bViewerOptimizeMesh = false;
    m_bWriteResultsToLog = false;
}
//---------------------------------------------------------------------------
NiExporterOptionsSharedData::~NiExporterOptionsSharedData()
{
}
//---------------------------------------------------------------------------
NiScriptInfo* NiExporterOptionsSharedData::GetCurrentScript()
{
    return m_spCurrentScript;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetCurrentScript(NiScriptInfo* pkInfo)
{
    if (pkInfo != NULL)
        m_spCurrentScript = pkInfo;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetViewerRun()
{
    return m_bViewerRun;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetViewerRun(bool bViewerRun)
{
    m_bViewerRun = bViewerRun;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetOptimizeMeshForViewer()
{
    return m_bViewerOptimizeMesh;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetOptimizeMeshForViewer(
    bool bViewerOptimizeMesh)
{
    m_bViewerOptimizeMesh = bViewerOptimizeMesh;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetWriteResultsToLog()
{
    return m_bWriteResultsToLog;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetWriteResultsToLog(
    bool bWriteResultsToLog)
{
    m_bWriteResultsToLog = bWriteResultsToLog;
}
//---------------------------------------------------------------------------
NiXMLLogger* NiExporterOptionsSharedData::GetXMLLogger()
{
    return m_pkLogger;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetXMLLogger(NiXMLLogger* pkLogger)
{
    m_pkLogger = pkLogger;
}
//---------------------------------------------------------------------------
