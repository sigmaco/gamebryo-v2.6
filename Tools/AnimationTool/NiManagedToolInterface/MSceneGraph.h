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

#include "MSceneObject.h"

namespace NiManagedToolInterface
{
    public __gc class MSceneGraph
    {
    public:
        static const unsigned int INVALID_INDEX = (unsigned int) -1;

        // Events.
        __event void OnNIFLoaded(MSceneObject* pkLoadedScene);
        __event void OnNIFSaved(MSceneObject* pkSavedScene);
        __event void OnSceneRemoved(MSceneObject* pkSceneToBeRemoved);
        __event void OnSceneChanged(MSceneObject* pkChangedScene);

        // Properties.
        __property static MSceneGraph* get_Instance();
        __property unsigned int get_CharacterIndex();
        __property unsigned int get_BackgroundIndex();
        __property unsigned int get_MainCameraIndex();
        __property unsigned int get_OrbitIndex();
        __property unsigned int get_SceneCount();
        
        void Update(float fTime);

        unsigned int LoadAndAddNIF(String* strFilename);
        bool LoadNIF(unsigned int uiIndex, String* strFilename);
        bool SaveNIF(unsigned int uiIndex, String* strFilename);

        MSceneObject* GetScene(unsigned int uiIndex);
        void RemoveScene(unsigned int uiIndex);
        void SetScene(unsigned int uiIndex, MSceneObject* pkScene);
    protected:
        static MSceneGraph* ms_SceneGraph = NULL;
    };
}
