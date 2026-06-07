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

#ifndef NIEXPORTEROPTIONSSHAREDDATA_H
#define NIEXPORTEROPTIONSSHAREDDATA_H

#include <NiPluginToolkitDefinitions.h>
#include "NiStandardSharedDataLibType.h"
#include "NiSharedData.h"
#include <NiTStringMap.h>
#include <NiMain.h>
#include <NiScriptInfo.h>
#include "NiXMLLogger.h"

/// This class contains the current information about timing
/// for the animations in the scene graph
class NISTANDARDSHAREDDATA_ENTRY NiExporterOptionsSharedData 
    : public NiSharedData
{
    public:
        NiDeclareRTTI;
        /// Default constructor.
        NiExporterOptionsSharedData();

        /// Virtual destructor.
        virtual ~NiExporterOptionsSharedData();
        
        NiScriptInfo* GetCurrentScript();
        void SetCurrentScript(NiScriptInfo* pkInfo);
        
        bool GetViewerRun();
        void SetViewerRun(bool bViewerRun);

        bool GetOptimizeMeshForViewer();
        void SetOptimizeMeshForViewer(bool bViewerOptimizeMesh);

        bool GetWriteResultsToLog();
        void SetWriteResultsToLog(bool bWriteResultsToLog);

        NiXMLLogger* GetXMLLogger();
        void SetXMLLogger(NiXMLLogger* pkLogger);

    protected:
        NiScriptInfoPtr m_spCurrentScript;
        bool m_bViewerRun;
        bool m_bViewerOptimizeMesh;
        bool m_bWriteResultsToLog;
        NiXMLLogger* m_pkLogger;
};


NiSmartPointer(NiExporterOptionsSharedData);

#endif  // #ifndef NIEXPORTEROPTIONSSHAREDDATA_H
