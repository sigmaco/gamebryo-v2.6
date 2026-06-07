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

#ifndef NIEXTERNALASSETKFHANDLER_H
#define NIEXTERNALASSETKFHANDLER_H

#include "NiEntityLibType.h"
#include "NiExternalAssetHandler.h"
#include "NiExternalAssetParams.h"
#include <NiTMap.h>
#include <NiTPtrSet.h>
#include <NiActorManager.h>

class NiExternalAssetManager;

class NIENTITY_ENTRY NiExternalAssetKFHandler :
    public NiExternalAssetHandler
{
public:
    NiExternalAssetKFHandler(bool bCloneFromPristine = false);
    virtual ~NiExternalAssetKFHandler();

    static NiFixedString T_ASSETTYPE;

    // Errors
    static NiFixedString ERR_FILE_LOAD_FAILED;

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
    
    virtual NiBool Retrieve(NiExternalAssetParams* pkParams);

    virtual NiBool GetCloneFromPristine(NiExternalAssetParams* pkParams = 0);
    virtual NiBool SetCloneFromPristine(bool bCloneFromPristine,
        NiExternalAssetParams* pkParams = 0);

    // Flush out all Loaded assets
    virtual void Unload(NiExternalAssetParams* pkParams, 
        NiExternalAssetManager* pkAssetManager = 0);
    virtual void UnloadAll(NiExternalAssetParams* pkParams = 0,
        NiExternalAssetManager* pkAssetManager = 0);
    virtual void UnloadAllUnusedAssets(NiExternalAssetParams* pkParams = 0,
        NiExternalAssetManager* pkAssetManager = 0);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    void Unload(const char* pcAssetName);

    NiBool LoadKFFile(const char* pcAssetName,
        NiEntityErrorInterface* pkErrorHandler,
        NiExternalAssetManager* pkAssetManager);

    // Store mapping from Asset path name to Asset
    NiTStringMap<NiTObjectArray<NiSequenceDataPtr>*> m_kSeqDataArrayMap;
   
    // CloneSet looks at each controller individually
    NiTStringMap<NiTObjectSet<NiSequenceDataPtr>*> m_kMapToCloneSet;

    bool m_bCloneFromPristine;
};

#endif // NIEXTERNALASSETKFHANDLER_H
