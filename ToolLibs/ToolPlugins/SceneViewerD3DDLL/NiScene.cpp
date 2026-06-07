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


#include "NiScene.h"

//---------------------------------------------------------------------------
NiScene::NiScene()
{
#if defined (WIN32)
    m_hIOMutex = NULL;
#endif
    m_spScene = NULL;
}
//---------------------------------------------------------------------------
NiScene::~NiScene()
{
#if defined (WIN32)
    CloseHandle(m_hIOMutex);
#endif
}
//---------------------------------------------------------------------------
bool NiScene::CheckOutSceneGraph(NiNode*& pkSceneRoot, bool bReturnImmediately)
{
#if defined (WIN32)
    pkSceneRoot = NULL;
    if (m_hIOMutex == NULL)
        m_hIOMutex= CreateMutex (NULL, FALSE, NULL);

    if (m_hIOMutex == NULL)
        return false;
    if (bReturnImmediately)
    {
        if (WaitForSingleObject(m_hIOMutex, 0) == WAIT_TIMEOUT)
            return false;
    }
    else
    {
        WaitForSingleObject( m_hIOMutex, INFINITE );
    }
#endif
    pkSceneRoot = m_spScene;
    return true;
}
//---------------------------------------------------------------------------
bool NiScene::CheckInSceneGraph(NiNode*& pkSceneRoot)
{
    m_spScene = pkSceneRoot;
    pkSceneRoot = NULL;
#if defined (WIN32)
    if (m_hIOMutex != NULL)
    {
        ReleaseMutex(m_hIOMutex);
        CloseHandle(m_hIOMutex);
        m_hIOMutex = NULL;
    }
#endif
    return true;
}
//---------------------------------------------------------------------------
void NiScene::DeleteScene()
{
#if defined (WIN32)
    if (m_hIOMutex != NULL)
        WaitForSingleObject(m_hIOMutex, INFINITE);
#endif
    //NIASSERT(m_spScene != NULL && m_spScene->GetRefCount() == 1);
    m_spScene = NULL;
}
//---------------------------------------------------------------------------
