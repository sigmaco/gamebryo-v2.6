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
#include "NiAnimationPCH.h"

#include "NiPoseBinding.h"
#include "NiPrimeNumber.h"
#include "NiSkinningLODController.h"
#include "NiControllerManager.h"

NiImplementRTTI(NiPoseBinding,NiObject);

//---------------------------------------------------------------------------
// NiPoseBinding
//---------------------------------------------------------------------------
NiPoseBinding::NiPoseBinding(NiControllerManager* pkManager, 
    const NiAVObject* pkAccumRoot) : 
    m_kAccumRootName(), m_usNonAccumStartIndex(0), 
    m_usNumTotalBindings(0), m_pusBindInfoIndexList(NULL),
    m_pkBindInfoList(NULL), m_usNumBindInfos(0), 
    m_usTableSize(0), m_pusHashTable(NULL), 
    m_usNumColorBindings(0), m_usColorStartIndex(0), 
    m_usNumBoolBindings(0), m_usBoolStartIndex(0), 
    m_usNumFloatBindings(0), m_usFloatStartIndex(0), 
    m_usNumPoint3Bindings(0), m_usPoint3StartIndex(0), 
    m_usNumRotBindings(0), m_usRotStartIndex(0), 
    m_usNumReferencedBindings(0), m_usReferencedStartIndex(0), 
    m_usNumLODs(1)
{
    Init(pkManager, pkAccumRoot);
}
//---------------------------------------------------------------------------
NiPoseBinding::~NiPoseBinding()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiPoseBinding::AddBindings(const NiSequenceData* pkSeqData,
    NiControllerManager* pkManager)
{
    // This function searches the evaluator list within pkSeqData for 
    // channels that are not yet bound to pose buffer locations. 
    // New bindings are created which map these new channels to new 
    // pose buffer locations. 
    //
    // pkManager specifies the controller manager on which pkSeqData 
    // will be used.

    NIASSERT(pkSeqData);
    NIASSERT(pkManager);

    // Ensure the hash table matches one of the sizes defined 
    // in NiEvaluator.
    if (m_usTableSize <= SMALLHASHTABLESIZE)
    {
        unsigned int uiTableSize = pkSeqData->GetNumEvaluators();
        if (uiTableSize < m_usNumBindInfos)
        {
            uiTableSize = m_usNumBindInfos;
        }
        if (uiTableSize <= SMALLHASHTABLESIZE + 5)
        {
            uiTableSize = SMALLHASHTABLESIZE;
        }
        else
        {
            uiTableSize = LARGEHASHTABLESIZE;
        }
        ReallocHashTable(uiTableSize);
    }
    NIASSERT(m_usTableSize > 0 && m_pusHashTable);

    // Determine the number of new items for each data type
    // required by this sequence data.
    unsigned short usNewColorBindings = 0;
    unsigned short usNewBoolBindings = 0;
    unsigned short usNewFloatBindings = 0;
    unsigned short usNewPoint3Bindings = 0;
    unsigned short usNewRotBindings = 0;
    unsigned short usNewReferencedBindings = 0;
    unsigned short usNewBindInfos = 0;

    unsigned int uiNumEvaluators = pkSeqData->GetNumEvaluators();
    NiUInt8* aucRequiresNewBinding = NULL;
    if (uiNumEvaluators > 0)
    {
        // Initialize the new binding array to indicate no evaluators
        // require new bindings.
        aucRequiresNewBinding = NiStackAlloc(NiUInt8, uiNumEvaluators);
        memset(aucRequiresNewBinding, 0, sizeof(NiUInt8) * uiNumEvaluators);
    }

    for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(ui);
        NIASSERT(pkEvaluator);

        const NiEvaluator::IDTag& kIDTag = pkEvaluator->GetIDTag();
        BindInfo* pkBindInfo = GetBindInfo(kIDTag);
        if (!pkBindInfo)
        {
            usNewBindInfos++;
        }

        NiPoseBufferChannelType ePBChannelType = 
            pkEvaluator->GetEvalPBChannelType(EVALBASEINDEX);
        if ((ePBChannelType != PBINVALIDCHANNEL) && 
            (!pkBindInfo || 
            !pkBindInfo->GetPBHandle(ePBChannelType).IsValid()))
        {
            switch(ePBChannelType)
            {
            case PBCOLORCHANNEL:
                usNewColorBindings++;
                break;
            case PBBOOLCHANNEL:
                usNewBoolBindings++;
                break;
            case PBFLOATCHANNEL:
                usNewFloatBindings++;
                break;
            case PBPOINT3CHANNEL:
                usNewPoint3Bindings++;
                break;
            case PBROTCHANNEL:
                usNewRotBindings++;
                break;
            case PBREFERENCEDCHANNEL:
                usNewReferencedBindings++;
                break;
            default:
                // PBINVALIDCHANNEL should not occur.
                NIASSERT(false);
            }
            // Mark evaluator as requiring a new binding.
            aucRequiresNewBinding[ui] = 1;
        }

        if (pkEvaluator->IsTransformEvaluator())
        {
            NIASSERT(EVALPOSINDEX == EVALBASEINDEX);
            // EVALPOSINDEX has already been handled.
            // Deal with the remaining two channels.
            ePBChannelType = pkEvaluator->GetEvalPBChannelType(
                EVALROTINDEX);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                NIASSERT(ePBChannelType == PBROTCHANNEL);
                if (!pkBindInfo || 
                    !pkBindInfo->GetPBHandle(PBROTCHANNEL).IsValid())
                {
                    usNewRotBindings++;
                    // Mark evaluator as requiring a new binding.
                    aucRequiresNewBinding[ui] = 1;
                }
            }

            ePBChannelType = pkEvaluator->GetEvalPBChannelType(
                EVALSCALEINDEX);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                NIASSERT(ePBChannelType == PBFLOATCHANNEL);
                if (!pkBindInfo || 
                    !pkBindInfo->GetPBHandle(PBFLOATCHANNEL).IsValid())
                {
                    usNewFloatBindings++;
                    // Mark evaluator as requiring a new binding.
                    aucRequiresNewBinding[ui] = 1;
                }
            }
        }
    }

    // Expand the array of bindings.
    unsigned short usNewTotalBindings = usNewColorBindings + 
        usNewBoolBindings + usNewFloatBindings + usNewPoint3Bindings +
        usNewRotBindings + usNewReferencedBindings;
    if (usNewTotalBindings == 0)
    {
        // No new bindings are required. Exit now.
        NiStackFree(aucRequiresNewBinding);
        return;
    }

    ReallocBindings(m_usNumColorBindings + usNewColorBindings,
        m_usNumBoolBindings + usNewBoolBindings,
        m_usNumFloatBindings + usNewFloatBindings,
        m_usNumPoint3Bindings + usNewPoint3Bindings,
        m_usNumRotBindings + usNewRotBindings,
        m_usNumReferencedBindings + usNewReferencedBindings, 
        m_usNumBindInfos + usNewBindInfos);

    NiAVObjectPalette* pkObjectPalette = pkManager->GetObjectPalette();
    NIASSERT(pkObjectPalette);

    NiSkinningLODController* pkLODController = 
        pkManager->GetSkinningLODController();

    // Fill in the new bindings in the expanded array.
    unsigned short usNumColorBindings = 
        m_usNumColorBindings - usNewColorBindings;
    unsigned short usNumBoolBindings = 
        m_usNumBoolBindings - usNewBoolBindings;
    unsigned short usNumFloatBindings = 
        m_usNumFloatBindings - usNewFloatBindings;
    unsigned short usNumPoint3Bindings = 
        m_usNumPoint3Bindings - usNewPoint3Bindings;
    unsigned short usNumRotBindings = 
        m_usNumRotBindings - usNewRotBindings;
    unsigned short usNumReferencedBindings = 
        m_usNumReferencedBindings - usNewReferencedBindings;
    unsigned short usNumBindInfos = 
        m_usNumBindInfos - usNewBindInfos;
    for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
    {
        if (aucRequiresNewBinding[ui] == 0)
        {
            continue;
        }

        NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(ui);
        NIASSERT(pkEvaluator)
        bool bTransformEval = pkEvaluator->IsTransformEvaluator();

        const NiEvaluator::IDTag& kIDTag = pkEvaluator->GetIDTag();
        BindInfo* pkBindInfo = GetBindInfo(kIDTag);
        if (!pkBindInfo)
        {
            // Init bind info from tag ID.
            pkBindInfo = m_pkBindInfoList + usNumBindInfos;
            pkBindInfo->m_kAVObjectName = kIDTag.GetAVObjectName();
            pkBindInfo->m_kPropertyType = kIDTag.GetPropertyType();
            pkBindInfo->m_kCtlrType = kIDTag.GetCtlrType();
            pkBindInfo->m_kCtlrID = kIDTag.GetCtlrID();
            pkBindInfo->m_kEvaluatorID = kIDTag.GetEvaluatorID();

            // Init LOD on transform channels.
            pkBindInfo->m_sLOD = (short)(m_usNumLODs - 1);
            if (pkLODController && bTransformEval)
            {
                NiAVObject* pkAVObject = pkObjectPalette->GetAVObject(
                    kIDTag.GetAVObjectName());
                if (pkAVObject)
                {
                    pkBindInfo->m_sLOD = (short)
                        pkLODController->GetLODForNode(pkAVObject);
                }
            }

            // Add bind info to the hash table.
            unsigned short usHashIndex;
            if (m_usTableSize == SMALLHASHTABLESIZE)
            {
                usHashIndex = kIDTag.GetSmallHashTableValue();
            }
            else
            {
                NIASSERT(m_usTableSize == LARGEHASHTABLESIZE);
                usHashIndex = kIDTag.GetLargeHashTableValue();
            }
            pkBindInfo->m_usNextHashIndex = m_pusHashTable[usHashIndex];
            m_pusHashTable[usHashIndex] = usNumBindInfos;
            usNumBindInfos++;

            NIASSERT(!pkBindInfo->m_kPBHandle.IsValid());
            NIASSERT(pkBindInfo->m_usPBIndices[0] == INVALIDCHANNELINDEX);
            NIASSERT(pkBindInfo->m_usPBIndices[1] == INVALIDCHANNELINDEX);
        }

        NiPoseBufferChannelType ePBChannelType = 
            pkEvaluator->GetEvalPBChannelType(EVALBASEINDEX);
        if ((ePBChannelType != PBINVALIDCHANNEL) && 
            (!pkBindInfo->GetPBHandle(ePBChannelType).IsValid()))
        {
            unsigned short* pusBindInfoIndex;
            NiPoseBufferHandle kPBHandle;
            switch(ePBChannelType)
            {
            case PBCOLORCHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usColorStartIndex + usNumColorBindings;
                kPBHandle.Init(PBCOLORCHANNEL, usNumColorBindings);
                usNumColorBindings++;
                break;
            case PBBOOLCHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usBoolStartIndex + usNumBoolBindings;
                kPBHandle.Init(PBBOOLCHANNEL, usNumBoolBindings);
                usNumBoolBindings++;
                break;
            case PBFLOATCHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usFloatStartIndex + usNumFloatBindings;
                kPBHandle.Init(PBFLOATCHANNEL, usNumFloatBindings);
                usNumFloatBindings++;
                break;
            case PBPOINT3CHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usPoint3StartIndex + usNumPoint3Bindings;
                kPBHandle.Init(PBPOINT3CHANNEL, usNumPoint3Bindings);
                usNumPoint3Bindings++;
                break;
            case PBROTCHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usRotStartIndex + usNumRotBindings;
                kPBHandle.Init(PBROTCHANNEL, usNumRotBindings);
                usNumRotBindings++;
                break;
            case PBREFERENCEDCHANNEL:
                pusBindInfoIndex = m_pusBindInfoIndexList + 
                    m_usReferencedStartIndex + usNumReferencedBindings;
                kPBHandle.Init(PBREFERENCEDCHANNEL, usNumReferencedBindings);
                usNumReferencedBindings++;
                break;
            default:
                // PBINVALIDCHANNEL should not occur.
                NIASSERT(false);
                continue;
            }

            // Init the index so it refers to the associated bind info.
            *pusBindInfoIndex = (unsigned short)(pkBindInfo - 
                m_pkBindInfoList);

            // Add the pose buffer handle to the bind info.
            pkBindInfo->AddPBHandle(kPBHandle);
        }

        if (bTransformEval)
        {
            NIASSERT(EVALPOSINDEX == EVALBASEINDEX);
            // EVALPOSINDEX has already been handled.
            // Deal with the remaining two channels.
            ePBChannelType = pkEvaluator->GetEvalPBChannelType(
                EVALROTINDEX);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                NIASSERT(ePBChannelType == PBROTCHANNEL);
                if (!pkBindInfo->GetPBHandle(PBROTCHANNEL).IsValid())
                {
                    // Init the index so it refers to the associated bind info.
                    m_pusBindInfoIndexList[
                        m_usRotStartIndex + usNumRotBindings] = 
                        (unsigned short)(pkBindInfo - m_pkBindInfoList);

                    // Add the pose buffer handle to the bind info.
                    pkBindInfo->AddPBHandle(
                        NiPoseBufferHandle(PBROTCHANNEL, usNumRotBindings));
                    usNumRotBindings++;
                }
            }

            ePBChannelType = pkEvaluator->GetEvalPBChannelType(
                EVALSCALEINDEX);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                NIASSERT(ePBChannelType == PBFLOATCHANNEL);
                if (!pkBindInfo->GetPBHandle(PBFLOATCHANNEL).IsValid())
                {
                    // Init the index so it refers to the associated bind info.
                    m_pusBindInfoIndexList[
                        m_usFloatStartIndex + usNumFloatBindings] =
                        (unsigned short)(pkBindInfo - m_pkBindInfoList);

                    // Add the pose buffer handle to the bind info.
                    pkBindInfo->AddPBHandle(NiPoseBufferHandle(
                        PBFLOATCHANNEL, usNumFloatBindings));
                    usNumFloatBindings++;
                }
            }
        }
    }

    // Verify that all the new bindings have been filled.
    NIASSERT(usNumColorBindings == m_usNumColorBindings);
    NIASSERT(usNumBoolBindings == m_usNumBoolBindings);
    NIASSERT(usNumFloatBindings == m_usNumFloatBindings);
    NIASSERT(usNumPoint3Bindings == m_usNumPoint3Bindings);
    NIASSERT(usNumRotBindings == m_usNumRotBindings);
    NIASSERT(usNumReferencedBindings == m_usNumReferencedBindings);
    NIASSERT(usNumBindInfos == m_usNumBindInfos);

    NiStackFree(aucRequiresNewBinding);
}
//---------------------------------------------------------------------------
NiPoseBufferHandle NiPoseBinding::AddBinding(BindInfo* pkBindInfo, 
    NiPoseBufferChannelType ePBChannelType)
{
    // This function augments an existing bind info with a new binding which 
    // maps the specified channel to a new pose buffer location.
    // A handle to the new location within the pose buffer is returned.

    NIASSERT(pkBindInfo);
    NIASSERT(ePBChannelType != PBINVALIDCHANNEL);
    NIASSERT(!pkBindInfo->GetPBHandle(ePBChannelType).IsValid());

    NiPoseBufferHandle kPBHandle;

    // Determine which data type receives a new binding.
    unsigned short usNewColorBindings = 0;
    unsigned short usNewBoolBindings = 0;
    unsigned short usNewFloatBindings = 0;
    unsigned short usNewPoint3Bindings = 0;
    unsigned short usNewRotBindings = 0;
    unsigned short usNewReferencedBindings = 0;
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        usNewColorBindings = 1;
        break;
    case PBBOOLCHANNEL:
        usNewBoolBindings = 1;
        break;
    case PBFLOATCHANNEL:
        usNewFloatBindings = 1;
        break;
    case PBPOINT3CHANNEL:
        usNewPoint3Bindings = 1;
        break;
    case PBROTCHANNEL:
        usNewRotBindings = 1;
        break;
    case PBREFERENCEDCHANNEL:
        usNewReferencedBindings = 1;
        break;
    default:
        // PBINVALIDCHANNEL should not occur.
        NIASSERT(false);
        return kPBHandle;
    }

    // Expand the array of bindings.
    NIASSERT(usNewColorBindings + usNewBoolBindings + usNewFloatBindings + 
        usNewPoint3Bindings + usNewRotBindings + usNewReferencedBindings == 1);
    ReallocBindings(m_usNumColorBindings + usNewColorBindings,
        m_usNumBoolBindings + usNewBoolBindings,
        m_usNumFloatBindings + usNewFloatBindings,
        m_usNumPoint3Bindings + usNewPoint3Bindings,
        m_usNumRotBindings + usNewRotBindings,
        m_usNumReferencedBindings + usNewReferencedBindings, 
        m_usNumBindInfos);

    // Initialize the new binding in the expanded array.

    unsigned short usNumColorBindings = 
        m_usNumColorBindings - usNewColorBindings;
    unsigned short usNumBoolBindings = 
        m_usNumBoolBindings - usNewBoolBindings;
    unsigned short usNumFloatBindings = 
        m_usNumFloatBindings - usNewFloatBindings;
    unsigned short usNumPoint3Bindings = 
        m_usNumPoint3Bindings - usNewPoint3Bindings;
    unsigned short usNumRotBindings = 
        m_usNumRotBindings - usNewRotBindings;
    unsigned short usNumReferencedBindings = 
        m_usNumReferencedBindings - usNewReferencedBindings;

    unsigned short* pusBindInfoIndex;
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usColorStartIndex + usNumColorBindings;
        kPBHandle.Init(PBCOLORCHANNEL, usNumColorBindings);
        usNumColorBindings++;
        break;
    case PBBOOLCHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usBoolStartIndex + usNumBoolBindings;
        kPBHandle.Init(PBBOOLCHANNEL, usNumBoolBindings);
        usNumBoolBindings++;
        break;
    case PBFLOATCHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usFloatStartIndex + usNumFloatBindings;
        kPBHandle.Init(PBFLOATCHANNEL, usNumFloatBindings);
        usNumFloatBindings++;
        break;
    case PBPOINT3CHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usPoint3StartIndex + usNumPoint3Bindings;
        kPBHandle.Init(PBPOINT3CHANNEL, usNumPoint3Bindings);
        usNumPoint3Bindings++;
        break;
    case PBROTCHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usRotStartIndex + usNumRotBindings;
        kPBHandle.Init(PBROTCHANNEL, usNumRotBindings);
        usNumRotBindings++;
        break;
    case PBREFERENCEDCHANNEL:
        pusBindInfoIndex = m_pusBindInfoIndexList + 
            m_usReferencedStartIndex + usNumReferencedBindings;
        kPBHandle.Init(PBREFERENCEDCHANNEL, usNumReferencedBindings);
        usNumReferencedBindings++;
        break;
    default:
        // PBINVALIDCHANNEL should not occur.
        NIASSERT(false);
        pusBindInfoIndex = NULL;
    }

    // Initialize the index so it refers to the associated bind info.
    *pusBindInfoIndex = (unsigned short)(pkBindInfo - 
        m_pkBindInfoList);

    // Add the pose buffer handle to the bind info.
    pkBindInfo->AddPBHandle(kPBHandle);

    // Verify the new binding has been filled.
    NIASSERT(usNumColorBindings == m_usNumColorBindings);
    NIASSERT(usNumBoolBindings == m_usNumBoolBindings);
    NIASSERT(usNumFloatBindings == m_usNumFloatBindings);
    NIASSERT(usNumPoint3Bindings == m_usNumPoint3Bindings);
    NIASSERT(usNumRotBindings == m_usNumRotBindings);
    NIASSERT(usNumReferencedBindings == m_usNumReferencedBindings);

    return kPBHandle;
}
//---------------------------------------------------------------------------
const NiPoseBinding::BindInfo* NiPoseBinding::GetBindInfo(
    NiPoseBufferHandle kPBHandle) const
{
    // This function finds the BindInfo associated with kPBHandle.
    // The pose buffer handle maps to a global index within 
    // m_pusBindInfoIndexList. The value of the corresponding entry 
    // can be used to acquire the associated BindInfo.

    if (!kPBHandle.IsValid())
    {
        return NULL;
    }

    // Compute the global index.
    NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    bool bValid = false;
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        bValid = (usIndex < m_usNumColorBindings);
        usIndex = (unsigned short)(usIndex + m_usColorStartIndex);
        break;
    case PBBOOLCHANNEL:
        bValid = (usIndex < m_usNumBoolBindings);
        usIndex = (unsigned short)(usIndex + m_usBoolStartIndex);
        break;
    case PBFLOATCHANNEL:
        bValid = (usIndex < m_usNumFloatBindings);
        usIndex = (unsigned short)(usIndex + m_usFloatStartIndex);
        break;
    case PBPOINT3CHANNEL:
        bValid = (usIndex < m_usNumPoint3Bindings);
        usIndex = (unsigned short)(usIndex + m_usPoint3StartIndex);
        break;
    case PBROTCHANNEL:
        bValid = (usIndex < m_usNumRotBindings);
        usIndex = (unsigned short)(usIndex + m_usRotStartIndex);
        break;
    case PBREFERENCEDCHANNEL:
        bValid = (usIndex < m_usNumReferencedBindings);
        usIndex = (unsigned short)(usIndex + m_usReferencedStartIndex);
        break;
    default:
        NIASSERT(false);
    }

    // If the global index is valid, find the associated BindInfo.
    if (bValid)
    {
        NIASSERT(m_pusBindInfoIndexList);
        NIASSERT(usIndex < m_usNumTotalBindings);
        unsigned short usBindInfoIndex = m_pusBindInfoIndexList[usIndex];
        NIASSERT(m_pkBindInfoList);
        NIASSERT(usBindInfoIndex < m_usNumBindInfos);
        return m_pkBindInfoList + usBindInfoIndex;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiPoseBinding::BindInfo* NiPoseBinding::GetBindInfo(
    const NiEvaluator::IDTag& kIDTag) const
{
    // This function finds the BindInfo associated with the specified 
    // evaluator ID tag. A hash function based on the ID tag is
    // used to speed the search.

    if (m_pkBindInfoList)
    {
        // Convert hash value to hash table index.
        unsigned short usHashIndex;
        if (m_usTableSize == SMALLHASHTABLESIZE)
        {
            usHashIndex = kIDTag.GetSmallHashTableValue();
        }
        else
        {
            NIASSERT(m_usTableSize == LARGEHASHTABLESIZE);
            usHashIndex = kIDTag.GetLargeHashTableValue();
        }
        // Search associated hash buckets.
        NIASSERT(m_pusHashTable);
        usHashIndex = m_pusHashTable[usHashIndex];
        while (usHashIndex != INVALIDHASHINDEX)
        {
            // Check for matching IDs.
            NIASSERT(usHashIndex < m_usNumBindInfos);
            BindInfo* pkBindInfo = m_pkBindInfoList + usHashIndex;
            if (pkBindInfo->MatchesIDTag(kIDTag))
            {
                return pkBindInfo;
            }
            usHashIndex = pkBindInfo->m_usNextHashIndex;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiPoseBinding::BindInfo* NiPoseBinding::GetBindInfo(
    const NiFixedString& kAVObjectName, const NiFixedString& kPropertyType, 
    const NiFixedString& kCtlrType, const NiFixedString& kCtlrID, 
    const NiFixedString& kEvaluatorID) const
{
    // This function finds the BindInfo associated with the specified 
    // evaluator ID tag. A hash function based on the ID tag is
    // used to speed the search.

    if (m_pkBindInfoList)
    {
        // Convert hash value to hash table index.
        NIASSERT(m_usTableSize > 0 && m_pusHashTable);
        unsigned short usHashIndex = NiEvaluator::IDTag::GetHashValue(
            kAVObjectName, kPropertyType, kCtlrType, kCtlrID, 
            kEvaluatorID) % m_usTableSize;

        // Search associated hash buckets.
        usHashIndex = m_pusHashTable[usHashIndex];
        while (usHashIndex != INVALIDHASHINDEX)
        {
            // Check for matching IDs.
            NIASSERT(usHashIndex < m_usNumBindInfos);
            BindInfo* pkBindInfo = m_pkBindInfoList + usHashIndex;
            if (pkBindInfo->MatchesIDTag(kAVObjectName, kPropertyType, 
                kCtlrType, kCtlrID, kEvaluatorID))
            {
                return pkBindInfo;
            }
            usHashIndex = pkBindInfo->m_usNextHashIndex;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPoseBinding::Init(NiControllerManager* pkManager, 
    const NiAVObject* pkAccumRoot)
{
    // pkManager is required to verify the accumulation root, if needed, 
    // and determine its associated level of detail.
    NIASSERT(pkManager);
    NIASSERT(pkManager->GetTarget());

    // Cache the number of LODs of the associated model.
    NiSkinningLODController* pkLODController = 
        pkManager->GetSkinningLODController();
    if (pkLODController)
    {
        m_usNumLODs = (unsigned short)pkLODController->GetNumberOfBoneLODs();
        if (m_usNumLODs < 1)
        {
            m_usNumLODs = 1;
        }
    }

    NIASSERT(m_usNonAccumStartIndex == 0);
    if (pkAccumRoot)
    {
        NIASSERT(pkAccumRoot == NiControllerManager::FindAccumRoot(
            (NiAVObject*)pkManager->GetTarget()));
        m_usNonAccumStartIndex = ACCUMENDINDEX;

        m_kAccumRootName = pkAccumRoot->GetName();
        NIASSERT(m_kAccumRootName.Exists());

        InitAccumRootBindings(pkManager, pkAccumRoot);
    }
}
//---------------------------------------------------------------------------
void NiPoseBinding::Shutdown()
{
    NiFree(m_pusBindInfoIndexList);
    m_pusBindInfoIndexList = NULL;

    NiDelete[] m_pkBindInfoList;
    m_pkBindInfoList = NULL;

    NiFree(m_pusHashTable);
    m_pusHashTable = NULL;

    m_usTableSize = 0; 
    m_usNumBindInfos = 0;
    m_usNumTotalBindings = 0;

    m_usNumColorBindings = 0;
    m_usColorStartIndex = 0;
    m_usNumBoolBindings = 0;
    m_usBoolStartIndex = 0;
    m_usNumFloatBindings = 0;
    m_usFloatStartIndex = 0;
    m_usNumPoint3Bindings = 0;
    m_usPoint3StartIndex = 0;
    m_usNumRotBindings = 0;
    m_usRotStartIndex = 0;
    m_usNumReferencedBindings = 0;
    m_usReferencedStartIndex = 0;
}
//---------------------------------------------------------------------------
void NiPoseBinding::InitAccumRootBindings(NiControllerManager* pkManager, 
    const NiAVObject* pkAccumRoot)
{
    NIASSERT(pkManager);
    NIASSERT(pkAccumRoot);
    NIASSERT(GetCumulativeAnimations());
    NIASSERT(m_kAccumRootName.Exists());

    // Ensure hash table has at least a few buckets.
    ReallocHashTable(SMALLHASHTABLESIZE);

    unsigned short usNewFloatBindings = ACCUMENDINDEX;
    unsigned short usNewPoint3Bindings = ACCUMENDINDEX;
    unsigned short usNewRotBindings = ACCUMENDINDEX;
    unsigned short usNewBindInfos = 1;

    // Check if the bindings already exist.
    BindInfo* pkBindInfo = GetBindInfo(m_kAccumRootName, NULL,
        NiAnimationConstants::GetTransformCtlrType(), NULL, NULL);
    if (pkBindInfo)
    {
        usNewBindInfos = 0;

        NiPoseBufferHandle kPBHandle = 
            pkBindInfo->GetPBHandle(PBFLOATCHANNEL);
        if (kPBHandle.IsValid())
        {
            usNewFloatBindings = 0;
        }
        kPBHandle = pkBindInfo->GetPBHandle(PBPOINT3CHANNEL);
        if (kPBHandle.IsValid())
        {
            usNewPoint3Bindings = 0;
        }
        kPBHandle = pkBindInfo->GetPBHandle(PBROTCHANNEL);
        if (kPBHandle.IsValid())
        {
            usNewRotBindings = 0;
        }
    }

    if (usNewFloatBindings + usNewPoint3Bindings + usNewRotBindings > 0)
    {
        // Expand the array of bindings.
        ReallocBindings(m_usNumColorBindings, m_usNumBoolBindings,
            m_usNumFloatBindings + usNewFloatBindings,
            m_usNumPoint3Bindings + usNewPoint3Bindings,
            m_usNumRotBindings + usNewRotBindings,
            m_usNumReferencedBindings, 
            m_usNumBindInfos + usNewBindInfos);

        if (!pkBindInfo)
        {
            // Init bind info from tag ID.
            NIASSERT(usNewBindInfos == 1);
            pkBindInfo = m_pkBindInfoList + m_usNumBindInfos - 1;
            pkBindInfo->m_kAVObjectName = m_kAccumRootName;
            pkBindInfo->m_kPropertyType = NULL;
            pkBindInfo->m_kCtlrType = 
                NiAnimationConstants::GetTransformCtlrType();
            pkBindInfo->m_kCtlrID = NULL;
            pkBindInfo->m_kEvaluatorID = NULL;

            // Init LOD on transform channels.
            NiSkinningLODController* pkLODController = 
                pkManager->GetSkinningLODController();
            if (pkLODController)
            {
                pkBindInfo->m_sLOD = (short)
                    pkLODController->GetLODForNode(pkAccumRoot);
            }
            else
            {
                pkBindInfo->m_sLOD = (short)(m_usNumLODs - 1);
            }
            NIASSERT(pkBindInfo->m_sLOD >= 0);

            // Add bind info to the hash table.
            NIASSERT(m_usTableSize > 0 && m_pusHashTable);
            unsigned short usHashIndex = NiEvaluator::IDTag::GetHashValue(
                m_kAccumRootName, NULL, 
                NiAnimationConstants::GetTransformCtlrType(), 
                NULL, NULL) % m_usTableSize;
            pkBindInfo->m_usNextHashIndex = m_pusHashTable[usHashIndex];
            m_pusHashTable[usHashIndex] = m_usNumBindInfos - 1;

            NIASSERT(!pkBindInfo->m_kPBHandle.IsValid());
            NIASSERT(pkBindInfo->m_usPBIndices[0] == INVALIDCHANNELINDEX);
            NIASSERT(pkBindInfo->m_usPBIndices[1] == INVALIDCHANNELINDEX);
        }

        NIASSERT(pkBindInfo);
        unsigned short usBindInfoIndex = 
            (unsigned short)(pkBindInfo - m_pkBindInfoList);
        if (usNewFloatBindings > 0)
        {
            // Move existing data.
            for (unsigned int ui = 
                m_usNumFloatBindings + m_usFloatStartIndex - 1;
                ui >= m_usFloatStartIndex + (unsigned int)ACCUMENDINDEX; ui--)
            {
                m_pusBindInfoIndexList[ui] = 
                    m_pusBindInfoIndexList[ui - ACCUMENDINDEX];
            }

            // Set the first two float indices. These correspond to the 
            // scale value and scale delta.
            m_pusBindInfoIndexList[m_usFloatStartIndex] = usBindInfoIndex;
            m_pusBindInfoIndexList[m_usFloatStartIndex + 1] = usBindInfoIndex;

            // Add the scale value (but not the scale delta) to the bind info.
            pkBindInfo->AddPBHandle(NiPoseBufferHandle(PBFLOATCHANNEL, 0));
        }

        if (usNewPoint3Bindings > 0)
        {
            // Move existing data.
            for (unsigned int ui = 
                m_usNumPoint3Bindings + m_usPoint3StartIndex - 1;
                ui >= m_usPoint3StartIndex + (unsigned int)ACCUMENDINDEX; ui--)
            {
                m_pusBindInfoIndexList[ui] = 
                    m_pusBindInfoIndexList[ui - ACCUMENDINDEX];
            }

            // Set the first two point3 indices. These correspond to the 
            // translate value and translate delta.
            m_pusBindInfoIndexList[m_usPoint3StartIndex] = usBindInfoIndex;
            m_pusBindInfoIndexList[m_usPoint3StartIndex + 1] = usBindInfoIndex;

            // Add the translate value (but not the translate delta) 
            // to the bind info.
            pkBindInfo->AddPBHandle(NiPoseBufferHandle(PBPOINT3CHANNEL, 0));
        }

        if (usNewRotBindings > 0)
        {
            // Move existing data.
            for (unsigned int ui = 
                m_usNumRotBindings + m_usRotStartIndex - 1;
                ui >= m_usRotStartIndex + (unsigned int)ACCUMENDINDEX; ui--)
            {
                m_pusBindInfoIndexList[ui] = 
                    m_pusBindInfoIndexList[ui - ACCUMENDINDEX];
            }

            // Set the first two rot indices. These correspond to the 
            // rotate value and rotate delta.
            m_pusBindInfoIndexList[m_usRotStartIndex] = usBindInfoIndex;
            m_pusBindInfoIndexList[m_usRotStartIndex + 1] = usBindInfoIndex;

            // Add the rotate value (but not the rotate delta) 
            // to the bind info.
            pkBindInfo->AddPBHandle(NiPoseBufferHandle(PBROTCHANNEL, 0));
        }
    }
}
//---------------------------------------------------------------------------
void NiPoseBinding::ReallocBindings(unsigned int uiNumColors,
    unsigned int uiNumBools, unsigned int uiNumFloats, 
    unsigned int uiNumPoint3s, unsigned int uiNumRots,
    unsigned int uiNumReferencedItems, unsigned int uiNumBindInfos)
{
    NIASSERT(uiNumColors >= m_usNumColorBindings);
    NIASSERT(uiNumBools >= m_usNumBoolBindings);
    NIASSERT(uiNumFloats >= m_usNumFloatBindings);
    NIASSERT(uiNumPoint3s >= m_usNumPoint3Bindings);
    NIASSERT(uiNumRots >= m_usNumRotBindings);
    NIASSERT(uiNumReferencedItems >= m_usNumReferencedBindings);
    NIASSERT(uiNumBindInfos >= m_usNumBindInfos);

    // Compute total bindings and allocate new bindings.
    unsigned short usNumTotalBindings = (unsigned short)(uiNumColors + 
        uiNumBools + uiNumFloats + uiNumPoint3s + uiNumRots + 
        uiNumReferencedItems);
    NIASSERT(usNumTotalBindings > m_usNumTotalBindings);
    unsigned short* pusNewIndexList = 
        NiAlloc(unsigned short, usNumTotalBindings);

    // Copy existing index data.
    unsigned short* pusNewIndex = pusNewIndexList;
    for (unsigned int ui = 0; ui < m_usNumTotalBindings; ui++)
    {
        if (ui == m_usBoolStartIndex)
        {
            pusNewIndex += (uiNumColors - m_usNumColorBindings);
        }
        if (ui == m_usFloatStartIndex)
        {
            pusNewIndex += (uiNumBools - m_usNumBoolBindings);
        }
        if (ui == m_usPoint3StartIndex)
        {
            pusNewIndex += (uiNumFloats - m_usNumFloatBindings);
        }
        if (ui == m_usRotStartIndex)
        {
            pusNewIndex += (uiNumPoint3s - m_usNumPoint3Bindings);
        }
        if (ui == m_usReferencedStartIndex)
        {
            pusNewIndex += (uiNumRots - m_usNumRotBindings);
        }
        *pusNewIndex = m_pusBindInfoIndexList[ui];
        pusNewIndex++;
    }

    // Delete existing index data.
    NiFree(m_pusBindInfoIndexList);

    // Use new bindings.
    m_pusBindInfoIndexList = pusNewIndexList;
    m_usNumTotalBindings = usNumTotalBindings;
    m_usNumColorBindings = (unsigned short)uiNumColors;
    m_usNumBoolBindings = (unsigned short)uiNumBools;
    m_usNumFloatBindings = (unsigned short)uiNumFloats;
    m_usNumPoint3Bindings = (unsigned short)uiNumPoint3s;
    m_usNumRotBindings = (unsigned short)uiNumRots;
    m_usNumReferencedBindings = (unsigned short)uiNumReferencedItems;
    NIASSERT(m_usColorStartIndex == 0);
    m_usBoolStartIndex = m_usColorStartIndex + m_usNumColorBindings;
    m_usFloatStartIndex = m_usBoolStartIndex + m_usNumBoolBindings;
    m_usPoint3StartIndex = m_usFloatStartIndex + m_usNumFloatBindings;
    m_usRotStartIndex = m_usPoint3StartIndex + m_usNumPoint3Bindings;
    m_usReferencedStartIndex = m_usRotStartIndex + m_usNumRotBindings;
    NIASSERT(m_usNumTotalBindings == m_usReferencedStartIndex + 
        m_usNumReferencedBindings);

    // Allocate new bind infos when needed.
    if (uiNumBindInfos > m_usNumBindInfos)
    {
        BindInfo* pkNewBindInfoList = NiNew BindInfo[uiNumBindInfos];

        BindInfo* pkNewBindInfo = pkNewBindInfoList;
        for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
        {
            *pkNewBindInfo = m_pkBindInfoList[ui];
            pkNewBindInfo++;
        }

        NiDelete[] m_pkBindInfoList;

        m_pkBindInfoList = pkNewBindInfoList;
        m_usNumBindInfos = (unsigned short)uiNumBindInfos;
    }
}
//---------------------------------------------------------------------------
void NiPoseBinding::ReallocHashTable(unsigned int uiTableSize)
{
    // Exit if table is already sufficiently large.
    if (uiTableSize <= m_usTableSize)
    {
        return;
    }

    // Ensure the table size is a prime number.
    NiPrimeNumber::GetNearbyPrime(uiTableSize, uiTableSize);

    // Delete the existing hash table.
    if (m_pusHashTable)
    {
        NiFree(m_pusHashTable);
        m_pusHashTable = NULL;
    }

    // Create the new hash table.
    m_pusHashTable = NiAlloc(unsigned short, uiTableSize);

    m_usTableSize = (unsigned short)uiTableSize;
    for (unsigned int ui = 0; ui < uiTableSize; ui++)
    {
        m_pusHashTable[ui] = INVALIDHASHINDEX;
    }

    // Add existing bind infos to the new hash table.
    NIASSERT(m_usTableSize > 0 && m_pusHashTable);
    for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
    {
        BindInfo& kBindInfo = m_pkBindInfoList[ui];
        unsigned short usHashIndex = NiEvaluator::IDTag::GetHashValue(
            kBindInfo.m_kAVObjectName, kBindInfo.m_kPropertyType, 
            kBindInfo.m_kCtlrType, kBindInfo.m_kCtlrID, 
            kBindInfo.m_kEvaluatorID) % m_usTableSize;
        kBindInfo.m_usNextHashIndex = m_pusHashTable[usHashIndex];
        m_pusHashTable[usHashIndex] = (unsigned short)ui;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPoseBinding);
//---------------------------------------------------------------------------
void NiPoseBinding::CopyMembers(NiPoseBinding* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kAccumRootName = m_kAccumRootName;
    pkDest->m_usNonAccumStartIndex = m_usNonAccumStartIndex;

    // Create the array of bindings.
    pkDest->ReallocBindings(m_usNumColorBindings, m_usNumBoolBindings, 
        m_usNumFloatBindings, m_usNumPoint3Bindings,
        m_usNumRotBindings, m_usNumReferencedBindings, m_usNumBindInfos);

    for (unsigned int ui = 0; ui < m_usNumTotalBindings; ui++)
    {
        pkDest->m_pusBindInfoIndexList[ui] = m_pusBindInfoIndexList[ui];
    }

    for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
    {
        pkDest->m_pkBindInfoList[ui] = m_pkBindInfoList[ui];
    }

    pkDest->m_usTableSize = m_usTableSize;
    if (m_usTableSize > 0)
    {
        pkDest->m_pusHashTable = NiAlloc(unsigned short, m_usTableSize);
        for (unsigned int ui = 0; ui < m_usTableSize; ui++)
        {
            pkDest->m_pusHashTable[ui] = m_pusHashTable[ui];
        }
    }
}
//---------------------------------------------------------------------------
void NiPoseBinding::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiPoseBinding::BindInfo::RegisterStreamables(NiStream& kStream)
{
    kStream.RegisterFixedString(m_kAVObjectName);
    kStream.RegisterFixedString(m_kPropertyType);
    kStream.RegisterFixedString(m_kCtlrType);
    kStream.RegisterFixedString(m_kCtlrID);
    kStream.RegisterFixedString(m_kEvaluatorID);
}
//---------------------------------------------------------------------------
void NiPoseBinding::BindInfo::SaveBinary(NiStream& kStream)
{
    kStream.SaveFixedString(m_kAVObjectName);
    kStream.SaveFixedString(m_kPropertyType);
    kStream.SaveFixedString(m_kCtlrType);
    kStream.SaveFixedString(m_kCtlrID);
    kStream.SaveFixedString(m_kEvaluatorID);
    NiStreamSaveBinary(kStream, m_sLOD);
    NiStreamSaveBinary(kStream, m_usNextHashIndex);
    m_kPBHandle.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_usPBIndices[0]);
    NiStreamSaveBinary(kStream, m_usPBIndices[1]);
}
//---------------------------------------------------------------------------
void NiPoseBinding::BindInfo::LoadBinary(NiStream& kStream)
{
    kStream.LoadFixedString(m_kAVObjectName);
    kStream.LoadFixedString(m_kPropertyType);
    kStream.LoadFixedString(m_kCtlrType);
    kStream.LoadFixedString(m_kCtlrID);
    kStream.LoadFixedString(m_kEvaluatorID);
    NiStreamLoadBinary(kStream, m_sLOD);
    NiStreamLoadBinary(kStream, m_usNextHashIndex);
    m_kPBHandle.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_usPBIndices[0]);
    NiStreamLoadBinary(kStream, m_usPBIndices[1]);
}
//---------------------------------------------------------------------------
bool NiPoseBinding::BindInfo::IsEqual(const BindInfo& kDest)
{
    if (m_kAVObjectName != kDest.m_kAVObjectName ||
        m_kPropertyType != kDest.m_kPropertyType ||
        m_kCtlrType != kDest.m_kCtlrType ||
        m_kCtlrID != kDest.m_kCtlrID ||
        m_kEvaluatorID != kDest.m_kEvaluatorID ||
        m_sLOD != kDest.m_sLOD)
    {
        if (m_kPBHandle != kDest.m_kPBHandle || 
            m_usPBIndices[0] != kDest.m_usPBIndices[0] ||
            m_usPBIndices[1] != kDest.m_usPBIndices[1])
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPoseBinding);
//---------------------------------------------------------------------------
void NiPoseBinding::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kAccumRootName);
    NiStreamLoadBinary(kStream, m_usNonAccumStartIndex);

    unsigned short usNumColorBindings;
    NiStreamLoadBinary(kStream, usNumColorBindings);
    unsigned short usNumBoolBindings;
    NiStreamLoadBinary(kStream, usNumBoolBindings);
    unsigned short usNumFloatBindings;
    NiStreamLoadBinary(kStream, usNumFloatBindings);
    unsigned short usNumPoint3Bindings;
    NiStreamLoadBinary(kStream, usNumPoint3Bindings);
    unsigned short usNumRotBindings;
    NiStreamLoadBinary(kStream, usNumRotBindings);
    unsigned short usNumReferencedBindings;
    NiStreamLoadBinary(kStream, usNumReferencedBindings);
    unsigned short usNumBindInfos;
    NiStreamLoadBinary(kStream, usNumBindInfos);

    // Create the array of bindings.
    ReallocBindings(usNumColorBindings, usNumBoolBindings, 
        usNumFloatBindings, usNumPoint3Bindings, usNumRotBindings, 
        usNumReferencedBindings, usNumBindInfos);

    unsigned short usNumTotalBindings;
    NiStreamLoadBinary(kStream, usNumTotalBindings);
    NIASSERT(usNumTotalBindings == m_usNumTotalBindings);
    for (unsigned int ui = 0; ui < usNumTotalBindings; ui++)
    {
        NiStreamLoadBinary(kStream, m_pusBindInfoIndexList[ui]);
    }

    for (unsigned int ui = 0; ui < usNumBindInfos; ui++)
    {
        m_pkBindInfoList[ui].LoadBinary(kStream);
    }

    NiStreamLoadBinary(kStream, m_usTableSize);
    if (m_usTableSize > 0)
    {
        m_pusHashTable = NiAlloc(unsigned short, m_usTableSize);
        for (unsigned int ui = 0; ui < m_usTableSize; ui++)
        {
            NiStreamLoadBinary(kStream, m_pusHashTable[ui]);
        }
    }
}
//---------------------------------------------------------------------------
void NiPoseBinding::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPoseBinding::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kAccumRootName);

    for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
    {
        m_pkBindInfoList[ui].RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPoseBinding::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kAccumRootName);
    NiStreamSaveBinary(kStream, m_usNonAccumStartIndex);

    NiStreamSaveBinary(kStream, m_usNumColorBindings);
    NiStreamSaveBinary(kStream, m_usNumBoolBindings);
    NiStreamSaveBinary(kStream, m_usNumFloatBindings);
    NiStreamSaveBinary(kStream, m_usNumPoint3Bindings);
    NiStreamSaveBinary(kStream, m_usNumRotBindings);
    NiStreamSaveBinary(kStream, m_usNumReferencedBindings);
    NiStreamSaveBinary(kStream, m_usNumBindInfos);

    NIASSERT(m_usNumTotalBindings == m_usNumColorBindings + 
        m_usNumBoolBindings + m_usNumFloatBindings + m_usNumPoint3Bindings +
        m_usNumRotBindings + m_usNumReferencedBindings);

    NiStreamSaveBinary(kStream, m_usNumTotalBindings);
    for (unsigned int ui = 0; ui < m_usNumTotalBindings; ui++)
    {
        NiStreamSaveBinary(kStream, m_pusBindInfoIndexList[ui]);
    }

    for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
    {
        m_pkBindInfoList[ui].SaveBinary(kStream);
    }

    NiStreamSaveBinary(kStream, m_usTableSize);
    for (unsigned int ui = 0; ui < m_usTableSize; ui++)
    {
        NiStreamSaveBinary(kStream, m_pusHashTable[ui]);
    }
}
//---------------------------------------------------------------------------
bool NiPoseBinding::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPoseBinding* pkPoseBinding = (NiPoseBinding*)pkObject;

    if (m_usNumTotalBindings != pkPoseBinding->m_usNumTotalBindings ||
        m_usNumColorBindings != pkPoseBinding->m_usNumColorBindings ||
        m_usNumBoolBindings != pkPoseBinding->m_usNumBoolBindings ||
        m_usNumFloatBindings != pkPoseBinding->m_usNumFloatBindings ||
        m_usNumPoint3Bindings != pkPoseBinding->m_usNumPoint3Bindings ||
        m_usNumRotBindings != pkPoseBinding->m_usNumRotBindings ||
        m_usNumReferencedBindings != 
        pkPoseBinding->m_usNumReferencedBindings ||
        m_usNumBindInfos != pkPoseBinding->m_usNumBindInfos ||
        m_kAccumRootName != pkPoseBinding->m_kAccumRootName ||
        m_usNonAccumStartIndex != pkPoseBinding->m_usNonAccumStartIndex)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_usNumBindInfos; ui++)
    {
        if (!m_pkBindInfoList[ui].IsEqual(pkPoseBinding->m_pkBindInfoList[ui]))
        {
            return false;
        }
    }

    // Ignore index list and hash table.

    return true;
}
//---------------------------------------------------------------------------
