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

#include "NiEntityErrorInterface.h"
#include "NiExternalAssetManager.h"
#include "NiExternalAssetKFMHandler.h"
#include "NiParamsKF.h"
#include "NiParamsNIF.h"
#include "NiParamsKFM.h"

NiFixedString NiExternalAssetKFMHandler::ERR_FILE_LOAD_FAILED;
NiFixedString NiExternalAssetKFMHandler::
    ERR_ACTOR_MANAGER_CREATION_FAILED;
NiFixedString NiExternalAssetKFMHandler::T_ASSETTYPE;

//---------------------------------------------------------------------------
NiExternalAssetKFMHandler::NiExternalAssetKFMHandler(bool
    bCloneFromPristine) : m_bCloneFromPristine(bCloneFromPristine) 
{
}
//---------------------------------------------------------------------------
NiExternalAssetKFMHandler::~NiExternalAssetKFMHandler()
{
    UnloadAll();
}
//---------------------------------------------------------------------------
NiFixedString NiExternalAssetKFMHandler::GetAssetType()
{
    return T_ASSETTYPE;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::CanHandleParams(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    if (NiIsKindOf(NiParamsKFM, pkParams))
        return true;
    else 
        return false;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::GetCloneFromPristine(
    NiExternalAssetParams*)
{
    return m_bCloneFromPristine;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::SetCloneFromPristine(
    bool bCloneFromPristine, NiExternalAssetParams*)
{
    // This function is only valid when no assets have already been loaded.
    if (m_kActorMap.GetCount() == 0 && m_kAccumActorMap.GetCount() == 0)
    {
        m_bCloneFromPristine = bCloneFromPristine;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int NiExternalAssetKFMHandler::GetNumAssetsRegistered(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKFM* pkKFMParams = (NiParamsKFM*)pkParams;

    if (pkKFMParams->GetAccumulated())
        return m_kAccumActorMap.GetCount();
    else
        return m_kActorMap.GetCount();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::Register(NiExternalAssetParams* pkParams)
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKFM* pkKFMParams = (NiParamsKFM*)pkParams;

    NiTStringMap<NiActorManagerPtr>* pkActorMap;

    if (pkKFMParams->GetAccumulated())
        pkActorMap = &m_kAccumActorMap;
    else
        pkActorMap = &m_kActorMap;

    const char* pcAssetName = pkParams->GetAssetPath();
    if (!pcAssetName)
    {
        return false;
    }

    NiActorManagerPtr spActor = NULL;
    if (pkActorMap->GetAt(pcAssetName, spActor))
    {
        // Already registered
        return true;
    }

    pkActorMap->SetAt(pcAssetName, NULL);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::LoadAll(NiExternalAssetParams* pkParams,
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

    NiTMapIterator kIter = m_kActorMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAssetName;
        NiActorManagerPtr spActor;
        m_kActorMap.GetNext(kIter, pcAssetName, spActor);

        if (spActor)
        {
            // Already loaded
            continue;
        }

        if (!LoadKFMFile(pcAssetName, false, pkErrorHandler, pkAssetManager))
            bSuccess = false;
    }

    kIter = m_kAccumActorMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAssetName;
        NiActorManagerPtr spActor;
        m_kAccumActorMap.GetNext(kIter, pcAssetName, spActor);

        if (spActor)
        {
            // Already loaded
            continue;
        }

        if (!LoadKFMFile(pcAssetName, true, pkErrorHandler, pkAssetManager))
            bSuccess = false;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::Load(NiExternalAssetParams* pkParams,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKFM* pkKFMParams = (NiParamsKFM*)pkParams;

    bool bAccum = pkKFMParams->GetAccumulated();
    const char* pcAssetName = pkKFMParams->GetAssetPath();

    return LoadKFMFile(pcAssetName, bAccum, pkErrorHandler, pkAssetManager);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::LoadKFMFile(const char* pcAssetName, 
    bool bAccum, NiEntityErrorInterface* pkErrorHandler, 
    NiExternalAssetManager* pkAssetManager)
{
    // Check to see if already loaded
    NiActorManagerPtr spActor = NULL;

    if (!bAccum)
    {
        NIVERIFY(m_kActorMap.GetAt(pcAssetName, spActor));
    }
    else
    {
        NIVERIFY(m_kAccumActorMap.GetAt(pcAssetName, spActor));
    }

    if (spActor)
    {
        return true; // Already loaded
    }

    // Create KFM tool and load KFM file.
    NiKFMToolPtr spKFMToolPtr = NiNew NiKFMTool;
    NiKFMTool::KFM_RC eRC = spKFMToolPtr->LoadFile(pcAssetName);
    if (eRC != NiKFMTool::KFM_SUCCESS || !spKFMToolPtr)
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_FILE_LOAD_FAILED, pcAssetName,
                "", "");
        }
        return false;
    }

    // Register Assets...
    NiActorManager::SequenceID* pSequenceIDs = NULL;
    unsigned int uiNumIDs = 0;

    if (pkAssetManager)
    {
        if (!RegisterAndResolveNIFsAndKFs(spKFMToolPtr, pkAssetManager,
            pkErrorHandler, pSequenceIDs, uiNumIDs))
        {
            // ERROR Reporting will have already been done.

            return false;
        }

        // Create without loading assets
        spActor = NiActorManager::Create(spKFMToolPtr, pcAssetName, 
            bAccum, false);

        if (spActor)
        {
            if (!LinkSequencesToActor(spKFMToolPtr, pkAssetManager,
                spActor, pSequenceIDs, uiNumIDs))
            {

                NiFree(pSequenceIDs);
                return false;
            }
        }
    }
    else
    {
        // Create with loading of assets
        spActor = NiActorManager::Create(spKFMToolPtr, pcAssetName, 
            bAccum, true);
    }

    // Ensure creation was successful
    if (!spActor)
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_ACTOR_MANAGER_CREATION_FAILED, 
                pcAssetName, NULL, NULL);
        }
        NiFree(pSequenceIDs);
        return false;
    }

    if (!bAccum)
    {
        // Directly set ActorManager Assets By grabbing from Asset Manager
        m_kActorMap.SetAt(pcAssetName, spActor);

        // Create clonearray for this asset
        NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
        NIASSERT(m_kMapToCloneSet.GetAt(pcAssetName, pkSet) == false);
        pkSet = NiNew NiTObjectPtrSet<NiActorManagerPtr>;
        m_kMapToCloneSet.SetAt(pcAssetName, pkSet);
    }
    else
    {
        // Directly set ActorManager Assets By grabbing from Asset Manager
        m_kAccumActorMap.SetAt(pcAssetName, spActor);

        // Create clonearray for this asset
        NiTObjectPtrSet<NiActorManagerPtr>* pkAccumSet = NULL;
        NIASSERT(m_kMapToAccumCloneSet.GetAt(pcAssetName, pkAccumSet) ==
            false);
        pkAccumSet = NiNew NiTObjectPtrSet<NiActorManagerPtr>;
        m_kMapToAccumCloneSet.SetAt(pcAssetName, pkAccumSet);
    }


    NiFree(pSequenceIDs);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::RegisterAndResolveNIFsAndKFs(
    NiKFMTool* pkKFMTool, NiExternalAssetManager* pkAssetManager,
    NiEntityErrorInterface*,
    NiActorManager::SequenceID*& pSequenceIDs, unsigned int& uiNumIDs)
{
    NIASSERT(pkAssetManager);
    bool bSuccess = true;

    // Register the NIF's that are found
    const NiFixedString& kFullPath = pkKFMTool->GetFullModelPath();
    NiParamsNIF kNIFParams;
    kNIFParams.SetAssetPath(kFullPath);
    if (!pkAssetManager->RegisterAndResolve(&kNIFParams))
    {
        // Nif handler would have reported errrors
        bSuccess = false;
    }

    pkKFMTool->GetSequenceIDs(pSequenceIDs, uiNumIDs);

    for(unsigned int ui=0; ui<uiNumIDs; ui++)
    {
        unsigned int uiSeqID = pSequenceIDs[ui];
        NiFixedString kKFPath = pkKFMTool->GetFullKFFilename(uiSeqID);

        if (!kKFPath.Exists())
            continue;

        NiParamsKF kKFParams;
        kKFParams.SetAssetPath(kKFPath);
        if (!pkAssetManager->RegisterAndResolve(&kKFParams))
        {
            // KF Handler would have reported errors
            bSuccess = false;
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::LinkSequencesToActor(
    NiKFMTool* pkKFMTool, NiExternalAssetManager* pkAssetManager, 
    NiActorManager* pkActor, const NiActorManager::SequenceID* pSequenceIDs, 
    unsigned int uiNumIDs)
{
    NIASSERT(pkAssetManager);

    // First, handle NIF file
    const char* pcFullPath = pkKFMTool->GetFullModelPath();
    if (pcFullPath == NULL)
        return false;
    
    NiParamsNIF kNIFParams;
    kNIFParams.SetAssetPath(pcFullPath);
    if (!pkAssetManager->Retrieve(&kNIFParams))
        return false;

    NiAVObject* pkObject = NULL;
    NIVERIFY(kNIFParams.GetSceneRoot(pkObject));

    if (!pkActor->ChangeNIFRoot(pkObject))
        return false;

    NIASSERT(pkActor->GetControllerManager());

    NiParamsKF kKFParams;
    for(unsigned int ui=0; ui<uiNumIDs; ui++)
    {
        unsigned int uiSeqID = pSequenceIDs[ui];
        NiKFMTool::Sequence* pkSeq = pkKFMTool->GetSequence(uiSeqID);
        NIASSERT(pkSeq);
        int iAnimIndex = pkSeq->GetAnimIndex();
        NiFixedString kKFPath = pkKFMTool->GetFullKFFilename(uiSeqID);
        NIASSERT(kKFPath.Exists());
        kKFParams.SetAssetPath(kKFPath);
        kKFParams.SetSequenceName(pkSeq->GetSequenceName());
        kKFParams.SetAnimIndex(iAnimIndex);
        NiBool bSuccess = pkAssetManager->Retrieve(&kKFParams);
        if (!bSuccess)
            return false;

        NiSequenceData* pkSeqData = NULL;
        NIVERIFY(kKFParams.GetSequenceData(pkSeqData));
        NIVERIFY(pkActor->ChangeSequenceData(uiSeqID, pkSeqData));
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFMHandler::Retrieve(NiExternalAssetParams* pkParams) 
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKFM* pkKFMParams = (NiParamsKFM*)pkParams;

    bool bAccum = pkKFMParams->GetAccumulated();
    const char* pcAssetName = pkParams->GetAssetPath();

    NiActorManagerPtr spActorPtr;

    NiTStringMap<NiActorManagerPtr>* pkActorMap;
    NiTStringMap<NiTObjectPtrSet<NiActorManagerPtr>*>* pkMapToCloneSet;

    if (!bAccum)
    {
        pkActorMap = &m_kActorMap;
        pkMapToCloneSet = &m_kMapToCloneSet;
    }
    else
    {
        pkActorMap = &m_kAccumActorMap;
        pkMapToCloneSet = &m_kMapToAccumCloneSet;
    }

    NIVERIFY(pkActorMap->GetAt(pcAssetName, spActorPtr));

    bool bNeedToClone = false;
    if (GetCloneFromPristine(pkKFMParams))
    {
        bNeedToClone = true;
    }
    else if (spActorPtr->GetRefCount() > 2)
    {
        // This is the case where CloneFromPristine is false and
        // a clone exists from the original.
        bNeedToClone = true;
    }

    if (bNeedToClone)
    {
        NiActorManagerPtr spActorClone = (NiActorManager*)spActorPtr->Clone();
        pkKFMParams->SetActor(spActorClone);

        // Record the clone
        NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
        NIVERIFY(pkMapToCloneSet->GetAt(pcAssetName, pkSet));
        NIASSERT(pkSet);
        pkSet->AddUnique(spActorClone);
    }
    else
    {
        pkKFMParams->SetActor(spActorPtr);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::Unload(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (!CanHandleParams(pkParams))
        return;

    NiParamsKFM* pkKFMParams = (NiParamsKFM*)pkParams;

    bool bAccum = pkKFMParams->GetAccumulated();
    const char* pcAssetName = pkParams->GetAssetPath();

    Unload(pcAssetName, bAccum);
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::UnloadAll(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    m_kActorMap.RemoveAll();
    m_kAccumActorMap.RemoveAll();

    NiTMapIterator kIter = m_kMapToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
        const char* pcAssetName;

        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        if (pkSet)
        {
            pkSet->RemoveAll();
            NiDelete pkSet;
        }
    }
    m_kMapToCloneSet.RemoveAll();

    kIter = m_kMapToAccumCloneSet.GetFirstPos();
    while(kIter)
    {
        NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
        const char* pcAssetName;

        m_kMapToAccumCloneSet.GetNext(kIter, pcAssetName, pkSet);

        if (pkSet)
        {
            pkSet->RemoveAll();
            NiDelete pkSet;
        }
    }
    m_kMapToAccumCloneSet.RemoveAll();
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::Unload(const char* pcAssetName, bool bAccum)
{
    NiTStringMap<NiActorManagerPtr>* pkActorMap;
    NiTStringMap<NiTObjectPtrSet<NiActorManagerPtr>*>* pkMapToCloneSet;

    if (bAccum)
    {
        pkActorMap = &m_kAccumActorMap;
        pkMapToCloneSet = &m_kMapToAccumCloneSet;
    }
    else
    {
        pkActorMap = &m_kActorMap;
        pkMapToCloneSet = &m_kMapToCloneSet;
    }

    if (pkActorMap)
        pkActorMap->RemoveAt(pcAssetName);
    NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
    bool bSuccess = pkMapToCloneSet->GetAt(pcAssetName, pkSet);
    if (bSuccess && pkSet)
    {
        NIVERIFY(pkMapToCloneSet->RemoveAt(pcAssetName));

        pkSet->RemoveAll();
        NiDelete pkSet;
    }
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::UnloadAllUnusedAssets(
    NiExternalAssetParams* pkParams, NiExternalAssetManager* pkAssetManager)
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

    for(int i=0; i<2; i++)
    {
        NiTStringMap<NiActorManagerPtr>* pkActorMap;
        NiTStringMap<NiTObjectPtrSet<NiActorManagerPtr>*>* pkMapToCloneSet;
        bool bAccum;

        if (i == 0)
        {
            pkActorMap = &m_kActorMap;
            pkMapToCloneSet = &m_kMapToCloneSet;
            bAccum = false;
        }
        else
        {
            pkActorMap = &m_kAccumActorMap;
            pkMapToCloneSet = &m_kMapToAccumCloneSet;
            bAccum = true;
        }


        NiTMapIterator kIter = pkMapToCloneSet->GetFirstPos();

        while(kIter)
        {
            NiTObjectPtrSet<NiActorManagerPtr>* pkSet = NULL;
            const char* pcAssetName;
            pkMapToCloneSet->GetNext(kIter, pcAssetName, pkSet);

            unsigned int uiSetSize = pkSet->GetSize();
            if (uiSetSize > 0)
            {
                for(int idx=uiSetSize-1; idx>=0; idx--)
                {
                    NiActorManager* pkActor = pkSet->GetAt(idx);
                    if (pkActor->GetRefCount() > 1)
                        continue;

                    // Remove the clone
                    pkSet->RemoveAt(idx);
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
                    Unload(pcAssetName, bAccum);
                }
                else
                {
                    // May have a clone from the non-pristine source.
                    NiActorManagerPtr spDirtyActor;
                    bool bSuccess = pkActorMap->GetAt(pcAssetName,
                        spDirtyActor);

                    if (bSuccess && spDirtyActor)
                    {
                        // Check if the only references that are stored are the
                        // ones this class is storing. If there are any 
                        // external references, we mark the asset as "dirty" 
                        // and keep the entire array. Otherwise, we unload it.
                        if (spDirtyActor->GetRefCount() == 2)
                        {
                            Unload(pcAssetName, bAccum);
                        }
                    }
                }
            }
        }
    }

    if (pkAssetManager)
    {
        // Because KFM Handler loading uses KF and NIF handlers and because
        // non-pristine controller sequences can't be readly re-assigned,
        // The unused NIF's and KF's should also be released. If the caller
        // provides pkAssetManager, then this will happen.
        NiExternalAssetHandler* pkHandler;
        
        NiParamsNIF kNIFParams;
        pkHandler = pkAssetManager->GetAssetHandler(&kNIFParams);
        pkHandler->UnloadAllUnusedAssets(&kNIFParams, pkAssetManager);

        NiParamsKF kKFParams;
        pkHandler = pkAssetManager->GetAssetHandler(&kKFParams);
        pkHandler->UnloadAllUnusedAssets(&kKFParams, pkAssetManager);
    }

    return;
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::_SDMInit()
{
    T_ASSETTYPE = "KFM";
    ERR_FILE_LOAD_FAILED = "KFM file load failed.";
    ERR_ACTOR_MANAGER_CREATION_FAILED = "The actor manager could not be "
        "created from the specified KFM file.";
}
//---------------------------------------------------------------------------
void NiExternalAssetKFMHandler::_SDMShutdown()
{
    T_ASSETTYPE = NULL;
    ERR_FILE_LOAD_FAILED = NULL;
    ERR_ACTOR_MANAGER_CREATION_FAILED = NULL;
}
//---------------------------------------------------------------------------
