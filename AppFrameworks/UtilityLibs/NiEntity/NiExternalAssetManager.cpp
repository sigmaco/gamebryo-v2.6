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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiFactories.h"
#include "NiExternalAssetManager.h"
#include "NiExternalAssetHandler.h"
#include "NiDefaultErrorHandler.h"
#include "NiExternalAssetParams.h"

//---------------------------------------------------------------------------
NiExternalAssetManager::NiExternalAssetManager(
    NiTFactory<NiExternalAssetHandler*>* pkFactory)
{
    m_spErrorHandler = NiNew NiDefaultErrorHandler();
    SetAssetFactory(pkFactory);
}
//---------------------------------------------------------------------------
NiExternalAssetManager::~NiExternalAssetManager()
{
    RemoveAll();

    m_spErrorHandler = 0;
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::SetAssetFactory(
    NiTFactory<NiExternalAssetHandler*>* pkFactory)
{
    m_pkAssetFactory = pkFactory;
}
//---------------------------------------------------------------------------
NiTFactory<NiExternalAssetHandler*>* NiExternalAssetManager::GetAssetFactory()
{
    return m_pkAssetFactory;
}
//---------------------------------------------------------------------------
NiExternalAssetHandler* NiExternalAssetManager::GetAssetHandler(
    NiExternalAssetParams* pkParams)
{
    // Traverse registered Makers in NiExternalAssetHandlerFactory
    // and check the "Type" field.

    NiTMapIterator kIter = GetAssetFactory()->GetFirstPos();

    while (kIter) 
    {
        NiFixedString kKey;
        GetAssetFactory()->GetNext(kIter, kKey);

        NiExternalAssetHandler* pkExternalAssetHandler =
            GetAssetFactory()->GetPersistent(kKey);
        NIASSERT(pkExternalAssetHandler);

        if (pkExternalAssetHandler->CanHandleParams(pkParams))
            return pkExternalAssetHandler;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetManager::Register(NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);

    if (!pkHandler)
        return false;

    return pkHandler->Register(pkParams);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetManager::RegisterAndResolve(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);

    if (!pkHandler)
        return false;

    if (!pkHandler->Register(pkParams))
        return false;

    // Load asset and store
    if (!pkHandler->Load(pkParams, m_spErrorHandler, this))
    {
        // This will ensure that asset is unregistered.
        pkHandler->Unload(pkParams); 
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetManager::Resolve(NiExternalAssetParams* pkParams)
{
    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);

    if (!pkHandler)
        return false;  

    // Load asset and store
    if (!pkHandler->Load(pkParams, m_spErrorHandler, this))
    {
        // Unload asset in it's handler
        pkHandler->Unload(pkParams); 
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetManager::ResolveAll(NiExternalAssetParams* pkParams)
{
    NiTMapIterator kIter = GetAssetFactory()->GetFirstPos();

    while (kIter) 
    {
        NiFixedString kKey;
        GetAssetFactory()->GetNext(kIter, kKey);

        NiExternalAssetHandler* pkExternalAssetHandler =
            GetAssetFactory()->GetPersistent(kKey);
        NIASSERT(pkExternalAssetHandler);

        if (!pkExternalAssetHandler->LoadAll(pkParams, m_spErrorHandler, 
            this))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// Used for retrieving resolved assets.
NiBool NiExternalAssetManager::Retrieve(NiExternalAssetParams* pkParams)
{
    // Get Handler for this type
    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);   

    if (pkHandler == NULL)
        return false; 

    // return a NiAVObject* 
    return pkHandler->Retrieve(pkParams);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetManager::Remove(NiExternalAssetParams* pkParams)
{
    // Get Handler for this type
    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);   
    if (pkHandler == NULL)
        return false; 

    // Unload asset
    pkHandler->Unload(pkParams); 

    return true;
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::RemoveAll(NiExternalAssetParams* pkParams)
{
    NiTFactory<NiExternalAssetHandler*>* pkAssetFactory = GetAssetFactory();

    if (pkAssetFactory == NULL)
        return;

    NiTMapIterator kIter = pkAssetFactory->GetFirstPos();

    while (kIter) 
    {
        NiFixedString kKey;
        pkAssetFactory->GetNext(kIter, kKey);

        NiExternalAssetHandler* pkExternalAssetHandler =
            pkAssetFactory->GetPersistent(kKey);
        NIASSERT(pkExternalAssetHandler);

        pkExternalAssetHandler->UnloadAll(pkParams);
    }
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::RemoveAllUnusedAssets(NiExternalAssetParams* 
    pkParams)
{
    NiTMapIterator kIter = GetAssetFactory()->GetFirstPos();

    while (kIter) 
    {
        NiFixedString kKey;
        GetAssetFactory()->GetNext(kIter, kKey);

        NiExternalAssetHandler* pkExternalAssetHandler =
            GetAssetFactory()->GetPersistent(kKey);
        NIASSERT(pkExternalAssetHandler);

        pkExternalAssetHandler->UnloadAllUnusedAssets(pkParams, this);
    }
}
//---------------------------------------------------------------------------
const NiEntityErrorInterface* NiExternalAssetManager::GetErrorHandler()
{
    return m_spErrorHandler;
}
//---------------------------------------------------------------------------
void NiExternalAssetManager::SetErrorHandler(
    NiEntityErrorInterface* pkHandler)
{
    m_spErrorHandler = pkHandler;
}
//---------------------------------------------------------------------------
unsigned int NiExternalAssetManager::GetNumAssetsRegistered(
    NiExternalAssetParams* pkParams) 
{
    NiExternalAssetHandler* pkHandler = GetAssetHandler(pkParams);

    if (pkHandler)
        return pkHandler->GetNumAssetsRegistered(pkParams);
    else
        return 0;
}
//---------------------------------------------------------------------------
