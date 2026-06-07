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

//---------------------------------------------------------------------------
// NiKFMTool constructor and destructor.
//---------------------------------------------------------------------------
inline NiKFMTool::NiKFMTool(const NiFixedString& kBaseKFMPath) : 
    m_kBaseKFMPath(kBaseKFMPath)
{
    m_pkDefaultSyncTrans = NiNew Transition(TYPE_MORPH, 0.25f);
    m_pkDefaultSyncTrans->m_eDefaultType = TYPE_DEFAULT_SYNC;
    m_pkDefaultNonSyncTrans = NiNew Transition(TYPE_CROSSFADE, 0.25f);
    m_pkDefaultNonSyncTrans->m_eDefaultType = TYPE_DEFAULT_NONSYNC;
}
//---------------------------------------------------------------------------
inline NiKFMTool::~NiKFMTool()
{
    unsigned int uiSequenceID;
    Sequence* pkSequence;
    NiTMapIterator pos = m_mapSequences.GetFirstPos();
    while (pos)
    {
        m_mapSequences.GetNext(pos, uiSequenceID, pkSequence);
        NiDelete pkSequence;
    }

    unsigned int uiGroupID;
    SequenceGroup* pkGroup;
    pos = m_mapSequenceGroups.GetFirstPos();
    while (pos)
    {
        m_mapSequenceGroups.GetNext(pos, uiGroupID, pkGroup);
        NiDelete pkGroup;
    }

    NiDelete m_pkDefaultSyncTrans;
    NiDelete m_pkDefaultNonSyncTrans;
}
//---------------------------------------------------------------------------
// NiKFMTool::Sequence class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::Sequence::Sequence() : m_uiSequenceID(0), m_iAnimIndex(0)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::Sequence::~Sequence()
{
    unsigned int uiSequenceID;
    Transition* pkTransition;
    NiTMapIterator pos = m_mapTransitions.GetFirstPos();
    while (pos)
    {
        m_mapTransitions.GetNext(pos, uiSequenceID, pkTransition);
        if (pkTransition->GetStoredType() != TYPE_DEFAULT_SYNC &&
            pkTransition->GetStoredType() != TYPE_DEFAULT_NONSYNC)
        {
            NiDelete pkTransition;
        }
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiKFMTool::Sequence::GetSequenceID() const
{
    return m_uiSequenceID;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Sequence::SetSequenceID(unsigned int uiSequenceID)
{
    m_uiSequenceID = uiSequenceID;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::Sequence::GetFilename() const
{
    return m_kFilename;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Sequence::SetFilename(const NiFixedString& kFilename)
{
    m_kFilename = kFilename;
}
//---------------------------------------------------------------------------
inline int NiKFMTool::Sequence::GetAnimIndex() const
{
    return m_iAnimIndex;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Sequence::SetAnimIndex(int iAnimIndex)
{
    m_iAnimIndex = iAnimIndex;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::Sequence::GetSequenceName() const
{
    return m_kSequenceName;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Sequence::SetSequenceName(
    const NiFixedString& kSequenceName)
{
    m_kSequenceName = kSequenceName;
}
//---------------------------------------------------------------------------
inline NiTPointerMap<unsigned int, NiKFMTool::Transition*>&
    NiKFMTool::Sequence::GetTransitions()
{
    return m_mapTransitions;
}
//---------------------------------------------------------------------------
// NiKFMTool::Transition class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::Transition() : m_eType(TYPE_BLEND),
    m_fDuration(0.0f), m_eDefaultType(TYPE_DEFAULT_INVALID)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::Transition(TransitionType eType,
    float fDuration) : m_eType(eType), m_fDuration(fDuration),
    m_eDefaultType(TYPE_DEFAULT_INVALID)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::~Transition()
{
    ClearBlendPairs();
}
//---------------------------------------------------------------------------
inline NiKFMTool::TransitionType NiKFMTool::Transition::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline NiKFMTool::TransitionType NiKFMTool::Transition::GetStoredType() const
{
    if (m_eDefaultType != TYPE_DEFAULT_INVALID)
    {
        return m_eDefaultType;
    }
    else
    {
        return m_eType;
    }
}
//---------------------------------------------------------------------------
inline float NiKFMTool::Transition::GetDuration() const
{
    return m_fDuration;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::SetDuration(float fDuration)
{
    m_fDuration = fDuration;
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::BlendPairSet& NiKFMTool::Transition::
    GetBlendPairs()
{
    return m_aBlendPairs;
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::ChainInfoSet& NiKFMTool::Transition::
    GetChainInfo()
{
    return m_aChainInfo;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::ClearBlendPairs()
{
    for (unsigned int ui = 0; ui < m_aBlendPairs.GetSize(); ui++)
    {
        NiDelete m_aBlendPairs.GetAt(ui);
    }
    m_aBlendPairs.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::ClearChainInfo()
{
    m_aChainInfo.RemoveAll();
}
//---------------------------------------------------------------------------
// NiKFMTool::Transition::BlendPair class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::BlendPair::BlendPair()
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::BlendPair::BlendPair(
    const NiFixedString& kStartKey, const NiFixedString& kTargetKey) 
{
    NIASSERT(kStartKey.Exists() ||  kTargetKey.Exists());
    SetStartKey(kStartKey);
    SetTargetKey(kTargetKey);
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::BlendPair::~BlendPair()
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::Transition::BlendPair::GetStartKey()
    const
{
    return m_kStartKey;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::BlendPair::SetStartKey(
    const NiFixedString& kStartKey)
{
    m_kStartKey = kStartKey;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::Transition::BlendPair::GetTargetKey()
    const
{
    return m_kTargetKey;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::BlendPair::SetTargetKey(
    const NiFixedString& kTargetKey)
{
    m_kTargetKey = kTargetKey;
}
//---------------------------------------------------------------------------
// NiKFMTool::Transition::ChainInfo class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::ChainInfo::ChainInfo() : m_uiSequenceID(0),
    m_fDuration(0.0f)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition::ChainInfo::ChainInfo(unsigned int uiSequenceID,
    float fDuration) : m_uiSequenceID(uiSequenceID), m_fDuration(fDuration)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiKFMTool::Transition::ChainInfo::GetSequenceID() const
{
    return m_uiSequenceID;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::ChainInfo::SetSequenceID(
    unsigned int uiSequenceID)
{
    m_uiSequenceID = uiSequenceID;
}
//---------------------------------------------------------------------------
inline float NiKFMTool::Transition::ChainInfo::GetDuration() const
{
    return m_fDuration;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::Transition::ChainInfo::SetDuration(float fDuration)
{
    m_fDuration = fDuration;
}
//---------------------------------------------------------------------------
// NiKFMTool::SequenceGroup class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::SequenceGroup() : m_uiGroupID(0)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::SequenceGroup(unsigned int uiGroupID,
    const NiFixedString& kName) : m_uiGroupID(uiGroupID)
{
    SetName(kName);
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::~SequenceGroup()
{
}
//---------------------------------------------------------------------------
inline unsigned int NiKFMTool::SequenceGroup::GetGroupID() const
{
    return m_uiGroupID;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SetGroupID(unsigned int uiGroupID)
{
    m_uiGroupID = uiGroupID;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::SequenceGroup::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::SequenceInfoSet& NiKFMTool::
    SequenceGroup::GetSequenceInfo()
{
    return m_aSequenceInfo;
}
//---------------------------------------------------------------------------
// NiKFMTool::SequenceGroup::SequenceInfo class implementation.
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::SequenceInfo::SequenceInfo() :
    m_uiSequenceID(0), m_iPriority(0), m_fWeight(0.0f), m_fEaseInTime(0.0f),
    m_fEaseOutTime(0.0f), m_uiSynchronizeSequenceID(SYNC_SEQUENCE_ID_NONE)
{
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup::SequenceInfo::SequenceInfo(
    unsigned int uiSequenceID, int iPriority, float fWeight,
    float fEaseInTime, float fEaseOutTime, 
    unsigned uiSynchronizedSequenceID) : m_uiSequenceID(uiSequenceID),
    m_iPriority(iPriority), m_fWeight(fWeight), m_fEaseInTime(fEaseInTime),
    m_fEaseOutTime(fEaseOutTime), 
    m_uiSynchronizeSequenceID(uiSynchronizedSequenceID)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiKFMTool::SequenceGroup::SequenceInfo::GetSequenceID()
    const
{
    return m_uiSequenceID;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetSequenceID(
    unsigned int uiSequenceID)
{
    m_uiSequenceID = uiSequenceID;
}
//---------------------------------------------------------------------------
inline int NiKFMTool::SequenceGroup::SequenceInfo::GetPriority() const
{
    return m_iPriority;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetPriority(int iPriority)
{
    m_iPriority = iPriority;
}
//---------------------------------------------------------------------------
inline float NiKFMTool::SequenceGroup::SequenceInfo::GetWeight() const
{
    return m_fWeight;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetWeight(float fWeight)
{
    m_fWeight = fWeight;
}
//---------------------------------------------------------------------------
inline float NiKFMTool::SequenceGroup::SequenceInfo::GetEaseInTime() const
{
    return m_fEaseInTime;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetEaseInTime(
    float fEaseInTime)
{
    m_fEaseInTime = fEaseInTime;
}
//---------------------------------------------------------------------------
inline float NiKFMTool::SequenceGroup::SequenceInfo::GetEaseOutTime() const
{
    return m_fEaseOutTime;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetEaseOutTime(
    float fEaseOutTime)
{
    m_fEaseOutTime = fEaseOutTime;
}
//---------------------------------------------------------------------------
inline unsigned int NiKFMTool::SequenceGroup::SequenceInfo::
    GetSynchronizeSequenceID() const
{
    return m_uiSynchronizeSequenceID;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SequenceGroup::SequenceInfo::SetSynchronizeSequenceID(
    unsigned int uiSynchronizeSequenceID)
{
    m_uiSynchronizeSequenceID = uiSynchronizeSequenceID;
}
//---------------------------------------------------------------------------
// Functions for retrieving components.
//---------------------------------------------------------------------------
inline NiKFMTool::Sequence* NiKFMTool::GetSequence(unsigned int uiSequenceID)
    const
{
    // Get specified sequence.
    return GetSequenceFromID(uiSequenceID);
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition* NiKFMTool::GetTransition(unsigned int uiSrcID,
    unsigned int uiDesID) const
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return NULL;
    }

    // Get specified transition.
    return GetTransitionFromID(uiDesID, pkSequence);
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup* NiKFMTool::GetSequenceGroup(
    unsigned int uiGroupID) const
{
    // Get specified sequence group.
    return GetSequenceGroupFromID(uiGroupID);
}
//---------------------------------------------------------------------------
// Functions for accessing model data.
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::GetModelPath() const
{
    return m_kModelPath;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SetModelPath(const NiFixedString& kModelPath)
{
    m_kModelPath = kModelPath;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::GetModelRoot() const
{
    return m_kModelRoot;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SetModelRoot(const NiFixedString& kModelRoot)
{
    m_kModelRoot = kModelRoot;
}
//---------------------------------------------------------------------------
// Functions for accessing default transition information.
//---------------------------------------------------------------------------
inline NiKFMTool::TransitionType NiKFMTool::GetDefaultSyncTransType() const
{
    return m_pkDefaultSyncTrans->GetType();
}
//---------------------------------------------------------------------------
inline NiKFMTool::KFM_RC NiKFMTool::SetDefaultSyncTransType(
    TransitionType eType)
{
    if (eType != TYPE_MORPH)
    {
        return KFM_ERR_SYNC_TRANS_TYPE;
    }

    m_pkDefaultSyncTrans->m_eType = eType;
    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
inline NiKFMTool::TransitionType NiKFMTool::GetDefaultNonSyncTransType() const
{
    return m_pkDefaultNonSyncTrans->GetType();
}
//---------------------------------------------------------------------------
inline NiKFMTool::KFM_RC NiKFMTool::SetDefaultNonSyncTransType(
    TransitionType eType)
{
    if (eType != TYPE_BLEND && eType != TYPE_CROSSFADE)
    {
        return KFM_ERR_NONSYNC_TRANS_TYPE;
    }

    m_pkDefaultNonSyncTrans->m_eType = eType;
    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
inline float NiKFMTool::GetDefaultSyncTransDuration() const
{
    return m_pkDefaultSyncTrans->GetDuration();
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SetDefaultSyncTransDuration(
    float fDuration)
{
    m_pkDefaultSyncTrans->SetDuration(fDuration);
}
//---------------------------------------------------------------------------
inline float NiKFMTool::GetDefaultNonSyncTransDuration() const
{
    return m_pkDefaultNonSyncTrans->GetDuration();
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SetDefaultNonSyncTransDuration(
    float fDuration)
{
    m_pkDefaultNonSyncTrans->SetDuration(fDuration);
}
//---------------------------------------------------------------------------
// Functions for performing lookups on components or component data.
//---------------------------------------------------------------------------
inline NiKFMTool::KFM_RC NiKFMTool::IsTransitionAllowed(unsigned int uiSrcID,
    unsigned int uiDesID, bool& bAllowed) const
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Check whether or not specified transition is present.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (pkTransition)
    {
        bAllowed = true;
    }
    else
    {
        bAllowed = false;
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
inline const char* NiKFMTool::LookupReturnCode(NiKFMTool::KFM_RC eReturnCode)
{
    switch (eReturnCode)
    {
        case KFM_SUCCESS:
            return "The operation completed successfully.";
            break;
        case KFM_ERROR:
            return "An unspecified error occurred.";
            break;
        case KFM_ERR_SEQUENCE:
            return "The specified sequence does not exist.";
            break;
        case KFM_ERR_TRANSITION:
            return "The specified transition does not exist.";
            break;
        case KFM_ERR_TRANSITION_TYPE:
            return "The specified transition is not of the correct type for "
                "this operation.";
            break;
        case KFM_ERR_BLEND_PAIR:
            return "The specified blend pair does not exist.";
            break;
        case KFM_ERR_NULL_TEXT_KEYS:
            return "Both of the specified text keys are null pointers.";
            break;
        case KFM_ERR_BLEND_PAIR_INDEX:
            return "The index specified for the blend pair is invalid.";
            break;
        case KFM_ERR_CHAIN_SEQUENCE:
            return "The ID specified for the chain sequence is invalid.";
            break;
        case KFM_ERR_SEQUENCE_IN_CHAIN:
            return "The specified sequence already exists in the specified "
                "chain.";
            break;
        case KFM_ERR_INFINITE_CHAIN:
            return "Adding the specified sequence to the chain would result "
                "in an infinite chain.";
            break;
        case KFM_ERR_SEQUENCE_GROUP:
            return "The specified sequence group does not exist.";
            break;
        case KFM_ERR_SEQUENCE_IN_GROUP:
            return "The specified sequence does not exist in the group.";
            break;
        case KFM_ERR_FILE_IO:
            return "A file I/O error occurred.";
            break;
        case KFM_ERR_FILE_FORMAT:
            return "The file is not in the correct format.";
            break;
        case KFM_ERR_FILE_VERSION:
            return "The file does not have a compatible version number.";
            break;
        case KFM_ERR_ENDIAN_MISMATCH:
            return "The file does not match the platform's endianness.";
            break;
        case KFM_ERR_SYNC_TRANS_TYPE:
            return "The transition type specified is not a synchronized "
                "transition type.";
            break;
        case KFM_ERR_NONSYNC_TRANS_TYPE:
            return "The transition type specified is not a non-synchronized "
                "transition type.";
            break;
        default:
            return "";
            break;
    }
}
//---------------------------------------------------------------------------
// Functions for getting fully qualified paths.
//---------------------------------------------------------------------------
inline const NiFixedString& NiKFMTool::GetBaseKFMPath() const
{
    return m_kBaseKFMPath;
}
//---------------------------------------------------------------------------
inline void NiKFMTool::SetBaseKFMPath(const NiFixedString& kBaseKFMPath)
{
    m_kBaseKFMPath = kBaseKFMPath;
}
//---------------------------------------------------------------------------
// Protected helper functions.
//---------------------------------------------------------------------------
inline NiKFMTool::Sequence* NiKFMTool::GetSequenceFromID(
    unsigned int uiSequenceID) const
{
    Sequence* pkSequence;
    if (m_mapSequences.GetAt(uiSequenceID, pkSequence))
    {
        return pkSequence;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiKFMTool::Transition* NiKFMTool::GetTransitionFromID(
    unsigned int uiSequenceID, Sequence* pkSequence) const
{
    NIASSERT(pkSequence);
    Transition* pkTransition;
    if (pkSequence->GetTransitions().GetAt(uiSequenceID, pkTransition))
    {
        return pkTransition;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiKFMTool::SequenceGroup* NiKFMTool::GetSequenceGroupFromID(
    unsigned int uiGroupID) const
{
    SequenceGroup* pkGroup;
    if (m_mapSequenceGroups.GetAt(uiGroupID, pkGroup))
    {
        return pkGroup;
    }

    return NULL;
}
//---------------------------------------------------------------------------
