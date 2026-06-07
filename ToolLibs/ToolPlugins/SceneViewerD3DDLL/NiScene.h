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

#ifndef NISCENE_H
#define NISCENE_H

#if defined (WIN32)
#include <windows.h>
#endif
#include <NiRefObject.h>
#include <NiNode.h>

class NiScene: public NiRefObject
{
public:
    NiScene();
    ~NiScene();

    bool CheckOutSceneGraph(NiNode*& pkSceneRoot, 
        bool bReturnImmediately = true);
    bool CheckInSceneGraph(NiNode*& pkSceneRoot);
    void DeleteScene();

protected:
    
#if defined (WIN32)
    HANDLE m_hIOMutex;
#endif
    NiNodePtr m_spScene;
};

NiSmartPointer(NiScene);

#endif