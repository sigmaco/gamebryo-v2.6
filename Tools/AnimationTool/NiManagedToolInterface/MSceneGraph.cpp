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
#include "MSceneGraph.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
void MSceneGraph::Update(float)
{
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::LoadAndAddNIF(String* strFilename)
{
    if (strFilename == String::Empty)
    {
        return INVALID_INDEX;
    }

    const char* pcFilename = MStringToCharPointer(strFilename);
    NiAVObjectPtr spScene = NULL;

    {
        NiStream kStream;
        int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream,
            pcFilename);
        MFreeCharPointer(pcFilename);
        if (iSuccess == 0)
        {
            return INVALID_INDEX;
        }
        spScene = NiDynamicCast(NiAVObject, kStream.GetObjectAt(0));
        if (!spScene)
        {
            return INVALID_INDEX;
        }
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiIndex = pkData->AddScene(spScene);
    pkData->Unlock();

    OnNIFLoaded(NiExternalNew MSceneObject(spScene));
    return uiIndex;
}
//---------------------------------------------------------------------------
bool MSceneGraph::LoadNIF(unsigned int uiIndex, String* strFilename)
{
    if (strFilename == String::Empty)
    {
        return false;
    }

    const char* pcFilename = MStringToCharPointer(strFilename);
    NiStream kStream;
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream,
        pcFilename);
    MFreeCharPointer(pcFilename);
    if (iSuccess == 0)
    {
        return false;
    }
    NiAVObject* pkScene = NiDynamicCast(NiAVObject, kStream.GetObjectAt(0));
    if (!pkScene)
    {
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiAVObjectPtr spOldScene = pkData->GetScene(uiIndex);
    pkData->SetScene(uiIndex, pkScene);
    pkData->Unlock();

    if (spOldScene)
    {
        OnSceneRemoved(NiExternalNew MSceneObject(spOldScene));
        pkData->Lock();
        spOldScene = NULL;
        pkData->Unlock();
    }

    OnNIFLoaded(NiExternalNew MSceneObject(pkScene));
    return true;
}
//---------------------------------------------------------------------------
bool MSceneGraph::SaveNIF(unsigned int uiIndex, String* strFilename)
{
    if (strFilename == String::Empty)
    {
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiAVObject* pkScene = pkData->GetScene(uiIndex);
    if (!pkScene)
    {
        pkData->Unlock();
        return false;
    }

    NiStream kStream;
    kStream.InsertObject(pkScene);
    const char* pcFilename = MStringToCharPointer(strFilename);
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Save(kStream,
        pcFilename);
    pkData->Unlock();
    MFreeCharPointer(pcFilename);
    if (iSuccess == 0)
    {
        return false;
    }

    OnNIFSaved(NiExternalNew MSceneObject(pkScene));
    return true;
}
//---------------------------------------------------------------------------
MSceneObject* MSceneGraph::GetScene(unsigned int uiIndex)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiAVObject* pkScene = pkData->GetScene(uiIndex);
    pkData->Unlock();

    if (!pkScene)
    {
        return NULL;
    }

    return NiExternalNew MSceneObject(pkScene);
}
//---------------------------------------------------------------------------
void MSceneGraph::RemoveScene(unsigned int uiIndex)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiAVObject* pkScene = pkData->GetScene(uiIndex);
    pkData->Unlock();

    if (!pkScene)
    {
        return;
    }

    OnSceneRemoved(NiExternalNew MSceneObject(pkScene));
    pkData->Lock();
    pkData->SetScene(uiIndex, NULL);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSceneGraph::SetScene(unsigned int uiIndex, MSceneObject* pkScene)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (uiIndex >= pkData->GetSceneArrayCount())
    {
        pkData->Unlock();
        return;
    }
    NiAVObjectPtr spOldScene = pkData->GetScene(uiIndex);
    pkData->SetScene(uiIndex, pkScene->GetObject());
    pkData->Unlock();

    if (spOldScene)
    {
        OnSceneRemoved(NiExternalNew MSceneObject(spOldScene));
        pkData->Lock();
        spOldScene = NULL;
        pkData->Unlock();
    }

    OnSceneChanged(pkScene);
}
//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
MSceneGraph* MSceneGraph::get_Instance()
{
    if (ms_SceneGraph == NULL)
        ms_SceneGraph = NiExternalNew MSceneGraph();
    return ms_SceneGraph;
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::get_CharacterIndex()
{
    return MSharedData::CHARACTER_INDEX;
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::get_BackgroundIndex()
{
    return MSharedData::BACKGROUND_INDEX;
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::get_MainCameraIndex()
{
    return MSharedData::MAIN_CAMERA_INDEX;
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::get_OrbitIndex()
{
    return MSharedData::ORBIT_INDEX;
}
//---------------------------------------------------------------------------
unsigned int MSceneGraph::get_SceneCount()
{
    unsigned int uiCount = 0;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    uiCount = pkData->GetSceneArrayCount();
    pkData->Unlock();
    return uiCount;
}
//---------------------------------------------------------------------------
