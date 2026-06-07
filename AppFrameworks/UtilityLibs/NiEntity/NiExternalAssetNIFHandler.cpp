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

#include "NiExternalAssetNIFHandler.h"
#include "NiParamsNIF.h"
#include "NiEntityErrorInterface.h"

#include <NiCloningProcess.h>

NiFixedString NiExternalAssetNIFHandler::T_ASSETTYPE;
NiFixedString NiExternalAssetNIFHandler::ERR_FILE_LOAD_FAILED;
NiFixedString NiExternalAssetNIFHandler::ERR_NIF_ROOT_OBJECT;

//---------------------------------------------------------------------------
NiExternalAssetNIFHandler::NiExternalAssetNIFHandler(bool bCloneFromPristine)
    : m_bCloneFromPristine(bCloneFromPristine)
{
}
//---------------------------------------------------------------------------
NiExternalAssetNIFHandler::~NiExternalAssetNIFHandler()
{
    UnloadAll();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::GetCloneFromPristine(
    NiExternalAssetParams*)
{
    return m_bCloneFromPristine;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::SetCloneFromPristine(
    bool bCloneFromPristine, NiExternalAssetParams*)
{
    // This function is only valid when no assets have already been loaded.
    if (m_kAVObjectMap.GetCount() == 0)
    {
        m_bCloneFromPristine = bCloneFromPristine;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::CanHandleParams(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    if (NiIsKindOf(NiParamsNIF, pkParams))
        return true;
    else 
        return false;
}
//---------------------------------------------------------------------------
NiFixedString NiExternalAssetNIFHandler::GetAssetType()
{
    return T_ASSETTYPE;
}
//---------------------------------------------------------------------------
unsigned int NiExternalAssetNIFHandler::GetNumAssetsRegistered(
    NiExternalAssetParams*)
{
    return m_kAVObjectMap.GetCount();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::Register(NiExternalAssetParams* pkParams)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiAVObjectPtr spAVObjectPtr;
    if (m_kAVObjectMap.GetAt(pcAssetName, spAVObjectPtr))
    {
        // Already registered
        return true;
    }

    m_kAVObjectMap.SetAt(pcAssetName, NULL);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::LoadAll(NiExternalAssetParams* pkParams,
    NiEntityErrorInterface* pkErrorHandler, 
    NiExternalAssetManager* pkAssetManager)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return false;
    }

    bool bSuccess = true; 

    NiTMapIterator kIter = m_kAVObjectMap.GetFirstPos();

    while(kIter)
    {
        const char* pcAssetName;
        NiAVObjectPtr spAVObject;
        m_kAVObjectMap.GetNext(kIter, pcAssetName, spAVObject);

        if (spAVObject)
        {
            // Already loaded
            continue;
        }

        if (!LoadNIFFile(pcAssetName, pkErrorHandler, pkAssetManager))
            bSuccess = false;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::Load(NiExternalAssetParams* pkParams,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiAVObjectPtr spAVObjectPtr = NULL;
    if (m_kAVObjectMap.GetAt(pcAssetName, spAVObjectPtr))
    {
        if (spAVObjectPtr)
        {
            // Already loaded
            return true;
        }
    }

    return LoadNIFFile(pcAssetName, pkErrorHandler, pkAssetManager);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::LoadNIFFile(const char* pcAssetName,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    m_kStream.RemoveAllObjects();

    if (!m_kStream.Load(pcAssetName))
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_FILE_LOAD_FAILED, pcAssetName,
                "", "");
        }
        return false;
    }

    NiAVObjectPtr spAVObject = NiDynamicCast(NiAVObject,
        m_kStream.GetObjectAt(0));

    if (!spAVObject)
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_NIF_ROOT_OBJECT, pcAssetName,
                "", "");
        }
        return false;
    }

    m_kAVObjectMap.SetAt(pcAssetName, spAVObject);

    // Create clonearray for this asset
    NiTObjectPtrSet<NiAVObjectPtr>* pkSet = NULL;
    NIASSERT(m_kMapToCloneSet.GetAt(pcAssetName, pkSet) == false);
    pkSet = NiNew NiTObjectPtrSet<NiAVObjectPtr>;
    m_kMapToCloneSet.SetAt(pcAssetName, pkSet);

    if (pkAssetManager)
    {
        // We can optionally RegisterAndResolve internal files here.
    }

    m_kStream.RemoveAllObjects();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetNIFHandler::Retrieve(NiExternalAssetParams* pkParams)
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsNIF* pkNIFParams = (NiParamsNIF*)pkParams;

    pkNIFParams->SetSceneRoot(NULL);

    const char* pcAssetName = pkParams->GetAssetPath();

    NiAVObjectPtr spAVObjectPtr;
    if (!m_kAVObjectMap.GetAt(pcAssetName, spAVObjectPtr))
        return false;

    if (spAVObjectPtr == NULL)
        return false;

    bool bNeedToClone = false;

    if (GetCloneFromPristine())
    {
        bNeedToClone = true;
    }
    else if (spAVObjectPtr->GetRefCount() > 2)
    {
        // This is the case where CloneFromPristine is false and
        // a clone exists from the original.
        bNeedToClone = true;
    }

    if (bNeedToClone)
    {
        // Cloning always occurs if uiClones is > 0
        // If uiClones is 0, Cloning occurs only if
        // GetCloneFromPristine is true.
        NiCloningProcess kCloning;
        kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
        NiAVObjectPtr spCloneObject = 
            (NiAVObject*)spAVObjectPtr->Clone(kCloning);
        pkNIFParams->SetSceneRoot(spCloneObject);

        // Record the clone
        NiTObjectPtrSet<NiAVObjectPtr>* pkSet = NULL;
        NIVERIFY(m_kMapToCloneSet.GetAt(pcAssetName, pkSet));
        NIASSERT(pkSet);
        pkSet->AddUnique(spCloneObject);
    }
    else
    {
        pkNIFParams->SetSceneRoot(spAVObjectPtr);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::Unload(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (!CanHandleParams(pkParams))
        return;

    const char* pcAssetName = pkParams->GetAssetPath();

    Unload(pcAssetName);
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::UnloadAll(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    m_kAVObjectMap.RemoveAll();

    NiTMapIterator kIter = m_kMapToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiTObjectPtrSet<NiAVObjectPtr>* pkSet = NULL;
        const char* pcAssetName;

        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        if (pkSet)
        {
            pkSet->RemoveAll();
            NiDelete pkSet;
        }
    }
    m_kMapToCloneSet.RemoveAll();
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::Unload(const char* pcAssetName)
{
    NIASSERT(pcAssetName);

    m_kAVObjectMap.RemoveAt(pcAssetName);

    NiTObjectPtrSet<NiAVObjectPtr>* pkSet = NULL;
    bool bSuccess = m_kMapToCloneSet.GetAt(pcAssetName, pkSet);
    if (bSuccess && pkSet)
    {
        NIVERIFY(m_kMapToCloneSet.RemoveAt(pcAssetName));
        pkSet->RemoveAll();
        NiDelete pkSet;
    }
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::UnloadAllUnusedAssets(NiExternalAssetParams* 
    pkParams, NiExternalAssetManager*)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    // This function will remove all unused clones.
    // Iterate through the clone map
    // For all clones that have only 1 ref count, unload this asset and
    // release all smartpointer references.

    NiTMapIterator kIter = m_kMapToCloneSet.GetFirstPos();

    while(kIter)
    {
        NiTObjectPtrSet<NiAVObjectPtr>* pkSet = NULL;
        const char* pcAssetName;
        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        unsigned int uiSetSize = pkSet->GetSize();
        if (uiSetSize > 0)
        {
            for(int i=uiSetSize-1; i>=0; i--)
            {
                NiAVObject* pkObject = pkSet->GetAt(i);
                if (pkObject->GetRefCount() > 1)
                    continue;

                // Remove the clone
                pkSet->RemoveAt(i);
            }
            uiSetSize = pkSet->GetSize();
        }

        if (uiSetSize == 0)
        {
            // If the size is 0 and clone from pristine is true, then the
            // asset is not being used. However, if clone from pristine is
            // false, then it is possible that there are no clones, yet the
            // original asset is being externally used. This is checked for
            // by analyzing the ref count.
            if (GetCloneFromPristine())
            {
                Unload(pcAssetName);
            }
            else
            {
                // May have a clone from the non-pristine source.
                NiAVObjectPtr spDirtyObject;
                bool bSuccess = m_kAVObjectMap.GetAt(pcAssetName,
                    spDirtyObject);

                if (bSuccess && spDirtyObject)
                {
                    // Check if the only references that are stored are the
                    // ones this class is storing. If there are any external
                    // references, we mark the asset as "dirty" and keep 
                    // the entire array. Otherwise, we unload it.
                    if (spDirtyObject->GetRefCount() == 2)
                    {
                        Unload(pcAssetName);                           
                    }
                }
            }
        }
    }

    return;
}
//---------------------------------------------------------------------------
NiStream& NiExternalAssetNIFHandler::GetReferenceToStream()
{
    return m_kStream;
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::_SDMInit()
{
    T_ASSETTYPE = "NIF";
    ERR_FILE_LOAD_FAILED = "NIF file load failed.";
    ERR_NIF_ROOT_OBJECT = "Root object at index 0 in NIF file is not an "
        "NiAVObject.";
}
//---------------------------------------------------------------------------
void NiExternalAssetNIFHandler::_SDMShutdown()
{
    T_ASSETTYPE = NULL;
    ERR_FILE_LOAD_FAILED = NULL;
    ERR_NIF_ROOT_OBJECT = NULL;
}
//---------------------------------------------------------------------------
