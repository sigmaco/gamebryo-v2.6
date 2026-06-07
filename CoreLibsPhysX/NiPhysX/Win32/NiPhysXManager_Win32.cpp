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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

//---------------------------------------------------------------------------
bool NiPhysXManager::PlatformSpecificPostSDKInit()
{
    // No Win32 specific initialization required.
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXManager::PlatformSpecificSDKShutdown()
{
    // Deliberately left empty.
}
//---------------------------------------------------------------------------
NxScene* NiPhysXManager::CreateSceneOnDifferentCore(
    const NxSceneDesc& kDesc, const NiProcessorAffinity&)
{
    // Simply create the scene. Windows will retarget the thread, and Intel
    // has actually advocated avoiding the use of affinities.
    NxScene* pkScene = m_pkPhysXSDK->createScene(kDesc);
    NIASSERT(pkScene);

    return pkScene;
}
//---------------------------------------------------------------------------