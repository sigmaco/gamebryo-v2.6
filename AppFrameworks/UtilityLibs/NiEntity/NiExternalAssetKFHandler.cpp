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

#include "NiExternalAssetKFHandler.h"
#include "NiParamsKF.h"
#include "NiEntityErrorInterface.h"

#include <NiCloningProcess.h>

NiFixedString NiExternalAssetKFHandler::T_ASSETTYPE;
NiFixedString NiExternalAssetKFHandler::ERR_FILE_LOAD_FAILED;

//---------------------------------------------------------------------------
NiExternalAssetKFHandler::NiExternalAssetKFHandler(bool bCloneFromPristine) :
    m_bCloneFromPristine(bCloneFromPristine) 
{
}
//---------------------------------------------------------------------------
NiExternalAssetKFHandler::~NiExternalAssetKFHandler()
{
    UnloadAll();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::GetCloneFromPristine(
    NiExternalAssetParams*)
{
    return m_bCloneFromPristine;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::SetCloneFromPristine(bool bCloneFromPristine,
    NiExternalAssetParams*)
{
    // This function is only valid when no assets have already been loaded.
    if (m_kSeqDataArrayMap.GetCount() == 0)
    {
        m_bCloneFromPristine = bCloneFromPristine;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::CanHandleParams(
    NiExternalAssetParams* pkParams)
{
    if (!pkParams)
        return false;

    if (NiIsKindOf(NiParamsKF, pkParams))
        return true;
    else 
        return false;
}
//---------------------------------------------------------------------------
NiFixedString NiExternalAssetKFHandler::GetAssetType()
{
    return T_ASSETTYPE;
}
//---------------------------------------------------------------------------
unsigned int NiExternalAssetKFHandler::GetNumAssetsRegistered(
    NiExternalAssetParams*)
{
    return m_kSeqDataArrayMap.GetCount();
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Register(NiExternalAssetParams* pkParams)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = NULL;
    if (m_kSeqDataArrayMap.GetAt(pcAssetName, pkSeqDataArray))
    {
        // Already registered
        return true;
    }

    m_kSeqDataArrayMap.SetAt(pcAssetName, NULL);
    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::LoadAll(
    NiExternalAssetParams* pkParams, NiEntityErrorInterface* pkErrorHandler, 
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

    NiTMapIterator kIter = m_kSeqDataArrayMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAssetName;
        NiTObjectArray<NiSequenceDataPtr>* pkSeqDataPtrArray;
        m_kSeqDataArrayMap.GetNext(kIter, pcAssetName, pkSeqDataPtrArray);

        if (pkSeqDataPtrArray)
        {
            // Already loaded
            continue;
        }

        if (!LoadKFFile(pcAssetName, pkErrorHandler, pkAssetManager))
            bSuccess = false;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Load(NiExternalAssetParams* pkParams,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager* pkAssetManager)
{
    if (!CanHandleParams(pkParams))
        return false;

    const char* pcAssetName = pkParams->GetAssetPath();

    NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = NULL;
    m_kSeqDataArrayMap.GetAt(pcAssetName, pkSeqDataArray);
    if (pkSeqDataArray)
    {
        return true; // Already loaded
    }

    return LoadKFFile(pcAssetName, pkErrorHandler, pkAssetManager);
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::LoadKFFile(const char* pcAssetName,
    NiEntityErrorInterface* pkErrorHandler,
    NiExternalAssetManager*)
{
    NiTObjectArray<NiSequenceDataPtr> kSeqDatas;
    NiBool bSuccess = NiSequenceData::CreateAllSequenceDatasFromFile(
        pcAssetName, kSeqDatas);

    if (!bSuccess)
    {
        if (pkErrorHandler)
        {
            pkErrorHandler->ReportError(ERR_FILE_LOAD_FAILED, pcAssetName,
                "","");
        }
        return false;
    }

    // Store kSeqDatas...
    NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = 
        NiNew NiTObjectArray<NiSequenceDataPtr>;
    pkSeqDataArray->SetSize(kSeqDatas.GetSize());
    for(unsigned int ui=0; ui<kSeqDatas.GetSize(); ui++)
    {
        pkSeqDataArray->SetAt(ui, kSeqDatas.GetAt(ui));
    }

    m_kSeqDataArrayMap.SetAt(pcAssetName, pkSeqDataArray);

    // Create clonearray for this asset
    NiTObjectSet<NiSequenceDataPtr>* pkSet = NULL;
    NIASSERT(m_kMapToCloneSet.GetAt(pcAssetName, pkSet) == false);
    pkSet = NiNew NiTObjectSet<NiSequenceDataPtr>;
    m_kMapToCloneSet.SetAt(pcAssetName, pkSet);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiExternalAssetKFHandler::Retrieve(NiExternalAssetParams* pkParams) 
{
    if (!CanHandleParams(pkParams))
        return false;

    NiParamsKF* pkKFParams = (NiParamsKF*)pkParams;

    const char* pcAssetName = pkParams->GetAssetPath();

    // Retrieve the sequence data array
    NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = NULL;
    if (m_kSeqDataArrayMap.GetAt(pcAssetName, pkSeqDataArray))
    {
        if (!pkSeqDataArray)
        {
            return false;
        }
    }

    unsigned int uiAnimIndex = 0;
    const NiFixedString& kSeqName = pkKFParams->GetSequenceName();
    if (kSeqName.Exists())
    {
        bool bFound = false;
        for (uiAnimIndex = 0; uiAnimIndex < pkSeqDataArray->GetSize();
            uiAnimIndex++)
        {
            if (pkSeqDataArray->GetAt(uiAnimIndex)->GetName() == kSeqName)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            NIASSERT(!"Couldn't find sequence by name");
            return false;
        }
    }
    else
    {        
        uiAnimIndex = pkKFParams->GetAnimIndex();
        if (uiAnimIndex >= pkSeqDataArray->GetSize())
        {
            NIASSERT(!"AnimIndex is out of range!");
            return false;
        }
    }


    bool bNeedToClone = false;
    if (GetCloneFromPristine())
    {
        // Always clone if pristine is set to true
        bNeedToClone = true;
    }
    else 
    {
        NiSequenceDataPtr spSeqData = (NiSequenceData*)
            pkSeqDataArray->GetAt(uiAnimIndex);

        if (spSeqData->GetRefCount() > 2)
        {
            // If the sequence data has an external reference (ie, if
            // the ref count is greater than 2: one for our local ref and
            // one for our stored ref), then cloning is needed. This is the
            // case where CloneFromPristine is false and a clone exists from
            // the original.
            bNeedToClone = true;
        }
    }

    if (bNeedToClone)
    {
        NiCloningProcess kCloning;
        kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
        NiSequenceDataPtr spSeqData = (NiSequenceData*)
            pkSeqDataArray->GetAt(uiAnimIndex)->Clone(kCloning);
        pkKFParams->SetSequenceData(spSeqData);

        // The clone is recorded to m_kMapToCloneSet
        NiTObjectSet<NiSequenceDataPtr>* pkSet = NULL;
        NIVERIFY(m_kMapToCloneSet.GetAt(pcAssetName, pkSet));
        NIASSERT(pkSet);
        pkSet->AddUnique(spSeqData);
    }
    else
    {
        pkKFParams->SetSequenceData(pkSeqDataArray->GetAt(uiAnimIndex));
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::Unload(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (!CanHandleParams(pkParams))
        return;

    const char* pcAssetName = pkParams->GetAssetPath();

    Unload(pcAssetName);
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::UnloadAll(NiExternalAssetParams* pkParams,
    NiExternalAssetManager*)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    // Remove all the sequence datas from each array before 
    // removing all the arrays.
    NiTMapIterator kIter = m_kSeqDataArrayMap.GetFirstPos();
    while(kIter)
    {
        const char* pcAsset;
        NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = NULL;
        m_kSeqDataArrayMap.GetNext(kIter, pcAsset, pkSeqDataArray);

        if (pkSeqDataArray)
            pkSeqDataArray->RemoveAll();

        NiDelete(pkSeqDataArray);
    }
    m_kSeqDataArrayMap.RemoveAll();

    kIter = m_kMapToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiTObjectSet<NiSequenceDataPtr>* pkSet = NULL;
        const char* pcAssetName;

        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        pkSet->RemoveAll();
        NiDelete pkSet;
    }
    m_kMapToCloneSet.RemoveAll();
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::Unload(const char* pcAssetName)
{
    if (pcAssetName)
    {
        NiTObjectArray<NiSequenceDataPtr>* pkSeqDataArray = NULL;
        bool bSuccess = m_kSeqDataArrayMap.GetAt(pcAssetName, pkSeqDataArray);

        // Remove all the sequence datas before removing the array
        if (bSuccess && pkSeqDataArray)
            pkSeqDataArray->RemoveAll();

        m_kSeqDataArrayMap.RemoveAt(pcAssetName);
    }

    NiTObjectSet<NiSequenceDataPtr>* pkSet = NULL;
    bool bSuccess = m_kMapToCloneSet.GetAt(pcAssetName, pkSet);
    if (bSuccess && pkSet)
    {
        NIVERIFY(m_kMapToCloneSet.RemoveAt(pcAssetName));

        pkSet->RemoveAll();
        NiDelete pkSet;
    }
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::UnloadAllUnusedAssets(
    NiExternalAssetParams* pkParams, NiExternalAssetManager*)
{
    if (pkParams)
    {
        // This function doesn't make use of pkParams, except for early
        // exiting if a param type was passed in that it can't handle.
        if (!CanHandleParams(pkParams))
            return;
    }

    // Iterate through the clone map
    // For all clones that have only 1 ref count, unload this asset and
    // release all smartpointer references.

    NiTMapIterator kIter = m_kMapToCloneSet.GetFirstPos();

    while(kIter)
    {
        NiTObjectSet<NiSequenceDataPtr>* pkSet = NULL;

        const char* pcAssetName;
        m_kMapToCloneSet.GetNext(kIter, pcAssetName, pkSet);

        unsigned int uiSetSize = pkSet->GetSize();
        if (uiSetSize > 0)
        {
            for(int i=uiSetSize-1; i>=0; i--)
            {
                NiSequenceData* pkSeqData = pkSet->GetAt(i);

                if (pkSeqData->GetRefCount() > 1)
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
                NiTObjectArray<NiSequenceDataPtr>* spDirtySeqArray =
                    NULL;

                bool bSuccess = m_kSeqDataArrayMap.GetAt(pcAssetName,
                    spDirtySeqArray);

                if (bSuccess && spDirtySeqArray)
                {
                    // Check if the only references that are stored are the
                    // ones this class is storing. If there are any external
                    // references, we mark the asset as "dirty" and keep 
                    // the entire array. Otherwise, we unload it.
                    // *Every* element must have a ref count of one in order
                    // to unload it.
                    bool bDirty = false;
                    unsigned int uiArraySize = spDirtySeqArray->GetSize();
                    for(unsigned int ui=0; ui<uiArraySize; ui++)
                    {
                        NiSequenceData* pkSeqData = 
                            spDirtySeqArray->GetAt(ui);

                        if (pkSeqData->GetRefCount() != 1)
                        {
                            bDirty = true;
                            break;
                        }
                    }

                    if (!bDirty)
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
void NiExternalAssetKFHandler::_SDMInit()
{
    T_ASSETTYPE = "KF";
    ERR_FILE_LOAD_FAILED = "KF file load failed.";
}
//---------------------------------------------------------------------------
void NiExternalAssetKFHandler::_SDMShutdown()
{
    T_ASSETTYPE = NULL;
    ERR_FILE_LOAD_FAILED = NULL;
}
//---------------------------------------------------------------------------
