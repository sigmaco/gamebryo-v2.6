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

#ifndef NIEXTERNALASSETHANDLER_H
#define NIEXTERNALASSETHANDLER_H

#include "NiSequenceData.h"
#include "NiEntityLibType.h"
#include "NiExternalAssetManager.h"
#include "NiRefObject.h"
#include <NiBool.h>
#include <NiFixedString.h>
#include <NiTArray.h>

class NiEntityErrorInterface;
class NiAVObject;
class NiKFMTool;
class NiActorManager;
class NiExternalAssetParams;

class NIENTITY_ENTRY NiExternalAssetHandler : public NiRefObject
{
public:
    NiExternalAssetHandler() {};
    inline virtual ~NiExternalAssetHandler() {};
   
    virtual NiBool CanHandleParams(NiExternalAssetParams* pkParams) = 0;

    virtual unsigned int GetNumAssetsRegistered(
        NiExternalAssetParams* pkParams) = 0;
    virtual NiBool Register(NiExternalAssetParams* pkParams) = 0;

    virtual NiBool LoadAll(NiExternalAssetParams* pkParams = 0,
        NiEntityErrorInterface* pkErrorHandler = 0, 
        NiExternalAssetManager* pkAssetManager = 0) = 0;
    virtual NiBool Load(NiExternalAssetParams* pkParams,
        NiEntityErrorInterface* pkErrorHandler = 0,
        NiExternalAssetManager* pkAssetManager = 0) = 0;
    virtual void Unload(NiExternalAssetParams* pkParams,
        NiExternalAssetManager* pkAssetManager = 0) = 0;
    virtual void UnloadAll(NiExternalAssetParams* pkParams = 0, 
        NiExternalAssetManager* pkAssetManager = 0) = 0;
    virtual void UnloadAllUnusedAssets(NiExternalAssetParams* pkParams = 0,
        NiExternalAssetManager* pkAssetManager = 0) = 0;
    virtual NiBool GetCloneFromPristine(NiExternalAssetParams* pkParams = 0)
        = 0;
    virtual NiBool SetCloneFromPristine(bool bCloneFromPristine, 
        NiExternalAssetParams* pkParams = 0) = 0;

    // Retrieval functions
    virtual NiBool Retrieve(NiExternalAssetParams* pkParams) = 0;
};

#endif // NIEXTERNALASSETHANDLER_H
