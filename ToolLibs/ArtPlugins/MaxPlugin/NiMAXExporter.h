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

#ifndef NIMAXEXPORTER_H
#define NIMAXEXPORTER_H

#include "MaxImmerse.h"
#include "NiMAXOptions.h"
#include "NiMAXConverter.h"

class NiKeyFrameExtractorSharedData;

class NiMAXExporter
{
    public:
        enum ExportTarget
        {
            TO_FILE = 0,
            TO_VIEWER,
            TO_VIEWPORT
        };

        MI_ENTRY static bool ExportScene(
            Interface* pkIntf, 
            ExportTarget eTarget,
            NiScriptInfo* pkTargetScriptInfo,
            bool bExportSelected = false);

    protected:
        static bool HandleScriptResults(
            NiBatchExecutionResult* pkResults, 
            NiScriptInfo* pkScript);

        static void SetupViewerRunStatics();
        static void SetupExportStatics();

        static bool AddCameras(
            NiNode* pkRoot,
            NiMAXConverter& kConverter, 
            bool bViewerRun);
        static bool AddSceneGraphSharedData(
            NiNode* pkRoot, 
            NiMAXConverter& kConverter, 
            bool bViewerRun);
        static bool AddAnimationSharedData(
            NiNode* pkRoot, 
            NiMAXConverter& kConverter, 
            bool bViewerRun);
        static bool AddRendererSharedData(
            NiNode* pkRoot, 
            NiMAXConverter& kConverter, 
            bool bViewerRun);

        static bool PostProcess(
            NiNode* pkRoot,
            NiMAXConverter& kConverter,
            Interface* pkIntf,
            NiScriptInfo* pkScript, 
            bool bViewerRun);
        static bool PreProcess(
            NiNode* pkRoot, 
            NiMAXConverter& kConverter,
            Interface* pkIntf, 
            NiScriptInfo* pkScript, 
            bool bViewerRun);

};

#endif