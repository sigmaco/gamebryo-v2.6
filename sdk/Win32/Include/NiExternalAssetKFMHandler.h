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

#ifndef NIEXTERNALASSETKFMHANDLER_H
#define NIEXTERNALASSETKFMHANDLER_H

#include "NiEntityLibType.h"
#include "NiExternalAssetHandler.h"
#include "NiExternalAssetParams.h"
#include <NiTMap.h>
#include <NiActorManager.h>
#include <NiTPtrSet.h>

class NiExternalAssetManager;
class NiExternalAssetParams;

class NIENTITY_ENTRY NiExternalAssetKFMHandler :
    public NiExternalAssetHandler
{
public:
    NiExternalAssetKFMHandler(bool bCloneFromPristine = false);
    virtual ~NiExternalAssetKFMHandler();

    static NiFixedString T_ASSETTYPE;

    // Errors
    static NiFixedString ERR_FILE_LOAD_FAILED;
    static NiFixedString ERR_ACTOR_MANAGER_CREATION_FAILED;

    virtual NiBool CanHandleParams(NiExternalAssetParams* pkParams);
    virtual NiFixedString GetAssetType();

    virtual unsigned int GetNumAssetsRegistered(
        NiExternalAssetParams* pkParams);
    virtual NiBool Register(NiExternalAssetParams* pkParams);

    virtual NiBool LoadAll(NiExternalAssetParams* pkParams = 0,
        NiEntityErrorInterface* pkErrorHandler = 0,
        NiExternalAssetManager* pkAssetManager = 0);

    virtual NiBool Load(NiExternalAssetParams* pkParams,
        NiEntityErrorInterface* pkErrorHandler = 0,
        NiExternalAssetManager* pkAssetManager = 0);
    virtual void Unload(NiExternalAssetParams* pkParams,
        NiExternalAssetManager* pkAssetManager = 0);
    virtual void UnloadAll(NiExternalAssetParams* pkParams = 0,
        NiExternalAssetManager* pkAssetManager = 0);
    virtual void UnloadAllUnusedAssets(NiExternalAssetParams* pkParams = 0,
        NiExternalAssetManager* pkAssetManager = 0);

    virtual NiBool GetCloneFromPristine(NiExternalAssetParams* pkParams = 0);
    virtual NiBool SetCloneFromPristine(bool bCloneFromPristine,
        NiExternalAssetParams* pkParams = 0);

    virtual NiBool Retrieve(NiExternalAssetParams* pkParams);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiBool RegisterAndResolveNIFsAndKFs(NiKFMTool* pkKFMTool, 
        NiExternalAssetManager* pkAssetManager,
        NiEntityErrorInterface* pkErrorHandler,
        NiActorManager::SequenceID*& pSequenceIDs,
        unsigned int& uiNumIDs);

    NiBool LinkSequencesToActor(NiKFMTool* pkKFMTool, 
        NiExternalAssetManager* pkAssetManager,
        NiActorManager* pkActor,
        const NiActorManager::SequenceID* pSequenceIDs,
        unsigned int uiNumIDs);

    NiBool LoadKFMFile(const char* pcAssetName, bool bAccum, 
        NiEntityErrorInterface* pkErrorHandler, 
        NiExternalAssetManager* pkAssetManager);
    
    void Unload(const char* pcAssetName, bool bAccum);

    // Store mapping from Asset path name to Asset
    NiTStringMap<NiActorManagerPtr> m_kActorMap;
    NiTStringMap<NiTObjectPtrSet<NiActorManagerPtr>*> m_kMapToCloneSet;

    NiTStringMap<NiActorManagerPtr> m_kAccumActorMap;
    NiTStringMap<NiTObjectPtrSet<NiActorManagerPtr>*> m_kMapToAccumCloneSet;

    bool m_bCloneFromPristine;
};

#endif // NIEXTERNALASSETKFMHANDLER_H
