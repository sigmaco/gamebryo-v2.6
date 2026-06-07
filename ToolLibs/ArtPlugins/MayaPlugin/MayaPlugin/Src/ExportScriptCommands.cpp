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

#include "MayaPluginPCH.h"
#include "ExportScriptCommands.h"
#include "maya/MArgList.h"

//---------------------------------------------------------------------------
MStatus OpenSelectScriptWindow::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);
    MStatus kStat = MStatus::kSuccess;

    NiScriptInfoPtr spSceneScriptInfo = MyImmerse::LoadScriptFromScene();
    if (!spSceneScriptInfo)
    {
        spSceneScriptInfo = NiScriptTemplateManager::GetInstance()->
            GetScript("Default Export");
    }

    // Give the User the option of selecting a different script
    NiScriptInfoPtr spScriptInfo = NiScriptInfoDialogs::DoSelectionDialog(
        spSceneScriptInfo, NULL, GetActiveWindow(), 
        "PROCESS\nEXPORT\nVIEWER")->Clone();
    if (spScriptInfo)
    {
        // Save the Script back into the scene
        MyImmerse::SaveScriptToScene(spScriptInfo);
    }

    return kStat;
}
//---------------------------------------------------------------------------
void* OpenSelectScriptWindow::creator()
{
    return new OpenSelectScriptWindow();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MStatus GetGamebryoRunSilently::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);
    bool bRunSilently = gExport.m_bRunSilently;
    setResult(bRunSilently);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* GetGamebryoRunSilently::creator()
{
    return new GetGamebryoRunSilently();
}
//---------------------------------------------------------------------------
MStatus SetGamebryoRunSilently::doIt( const MArgList& args )
{
    if (args.length() != 1)
    {
        MGlobal::executeCommand("warning \"Invalid argument true or false "
            "expected\n\";");
        return MStatus::kFailure;
    }
    gExport.m_bRunSilently = args.asBool(0);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* SetGamebryoRunSilently::creator()
{
    return new SetGamebryoRunSilently();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MStatus GetGamebryoUseLastSavedScript::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);
    bool bUseLastSavedScript = gExport.m_bUseLastSavedScript;
    setResult(bUseLastSavedScript);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* GetGamebryoUseLastSavedScript::creator()
{
    return new GetGamebryoUseLastSavedScript();
}
//---------------------------------------------------------------------------
MStatus SetGamebryoUseLastSavedScript::doIt( const MArgList& args )
{
    if (args.length() != 1)
    {
        MGlobal::executeCommand("warning \"Invalid argument true or false "
            "expected\n\";");
        return MStatus::kFailure;
    }
    gExport.m_bUseLastSavedScript = args.asBool(0);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* SetGamebryoUseLastSavedScript::creator()
{
    return new SetGamebryoUseLastSavedScript();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MStatus GetGamebryoForceDefaultScript::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);
    bool bForceDefaultScript = gExport.m_bForceDefaultScript;
    setResult(bForceDefaultScript);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* GetGamebryoForceDefaultScript::creator()
{
    return new GetGamebryoForceDefaultScript();
}
//---------------------------------------------------------------------------
MStatus SetGamebryoForceDefaultScript::doIt( const MArgList& args )
{
    if (args.length() != 1)
    {
        MGlobal::executeCommand("warning \"Invalid argument true or false "
            "expected\n\";");
        return MStatus::kFailure;
    }
    gExport.m_bForceDefaultScript = args.asBool(0);
    return MStatus::kSuccess;
}
//---------------------------------------------------------------------------
void* SetGamebryoForceDefaultScript::creator()
{
    return new SetGamebryoForceDefaultScript();
}
//---------------------------------------------------------------------------
