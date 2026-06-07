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

#ifndef NIEXTERNALASSETMANAGER_H
#define NIEXTERNALASSETMANAGER_H

#include "NiEntityErrorInterface.h"
#include "NiEntityLibType.h"
#include "NiTFactory.h"
#include <NiBool.h>
#include <NiTMap.h>
#include <NiSmartPointer.h>

class NiDefaultErrorHandler;
class NiExternalAssetHandler;
class NiExternalAssetParams;

class NIENTITY_ENTRY NiExternalAssetManager : public NiRefObject
{
public:
    NiExternalAssetManager(
        NiTFactory<NiExternalAssetHandler*>* pkFactory = NULL);
    virtual ~NiExternalAssetManager();

    virtual void AddReference();
    virtual void RemoveReference();

    // Used for adding an asset that will be resolved later.
    virtual NiBool Register(NiExternalAssetParams* pkParams);
    virtual NiBool RegisterAndResolve(NiExternalAssetParams* pkParams);
    virtual NiBool Resolve(NiExternalAssetParams* pkParams);
    virtual NiBool Retrieve(NiExternalAssetParams* pkParams);
    virtual NiBool Remove(NiExternalAssetParams* pkParams);
    virtual void RemoveAll(NiExternalAssetParams* pkParams = 0);
    virtual void RemoveAllUnusedAssets(NiExternalAssetParams* pkParams = 0);
    virtual NiBool ResolveAll(NiExternalAssetParams* pkParams = 0);

    virtual const NiEntityErrorInterface* GetErrorHandler();
    virtual void SetErrorHandler(NiEntityErrorInterface* pkHandler);

    virtual unsigned int GetNumAssetsRegistered(
        NiExternalAssetParams* pkParams);

    virtual NiExternalAssetHandler* GetAssetHandler(
        NiExternalAssetParams* pkParams);

    virtual void SetAssetFactory(NiTFactory<NiExternalAssetHandler*>*
        pkFactory);

    virtual NiTFactory<NiExternalAssetHandler*>* GetAssetFactory();

protected:
    // Error Handler
    NiEntityErrorInterfaceIPtr m_spErrorHandler;
    NiTFactory<NiExternalAssetHandler*>* m_pkAssetFactory;
};

NiSmartPointer(NiExternalAssetManager);

#endif // NIEXTERNALASSETMANAGER_H
