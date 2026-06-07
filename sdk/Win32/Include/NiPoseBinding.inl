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
// NiPoseBinding::BindInfo
//---------------------------------------------------------------------------
inline NiPoseBinding::BindInfo::BindInfo() :
    m_kAVObjectName(), m_kPropertyType(), m_kCtlrType(), 
    m_kCtlrID(), m_kEvaluatorID(), m_sLOD(0), 
    m_usNextHashIndex(INVALIDHASHINDEX), m_kPBHandle()
{
    m_usPBIndices[0] = INVALIDCHANNELINDEX;
    m_usPBIndices[1] = INVALIDCHANNELINDEX;
}
//---------------------------------------------------------------------------
inline NiPoseBufferHandle NiPoseBinding::BindInfo::GetFirstPBHandle() const
{
    NiPoseBufferHandle kPBHandle = m_kPBHandle;

    if (!m_kPBHandle.IsValid())
    {
        // The remaining two pose buffer handles for transform channels are 
        // stored as indices to conserve memory.
        if (m_usPBIndices[0] != INVALIDCHANNELINDEX)
        {
            NIASSERT(IsTransformData());
            kPBHandle.Init(PBPOINT3CHANNEL, m_usPBIndices[0]);
        }
        else if (m_usPBIndices[1] != INVALIDCHANNELINDEX)
        {
            NIASSERT(IsTransformData());
            kPBHandle.Init(PBFLOATCHANNEL, m_usPBIndices[1]);
        }
    }

    return kPBHandle;
}
//---------------------------------------------------------------------------
inline NiPoseBufferHandle NiPoseBinding::BindInfo::GetPBHandle(
    NiPoseBufferChannelType ePBChannelType) const
{
    NiPoseBufferHandle kPBHandle;

    if (m_kPBHandle.GetChannelTypeNoValidation() == ePBChannelType)
    {
        kPBHandle = m_kPBHandle;
    }
    // The remaining two pose buffer handles for transform channels are 
    // stored as indices to conserve memory.
    else if (m_usPBIndices[0] != INVALIDCHANNELINDEX &&
        ePBChannelType == PBPOINT3CHANNEL)
    {
        NIASSERT(IsTransformData());
        kPBHandle.Init(PBPOINT3CHANNEL, m_usPBIndices[0]);
    }
    else if (m_usPBIndices[1] != INVALIDCHANNELINDEX &&
        ePBChannelType == PBFLOATCHANNEL)
    {
        NIASSERT(IsTransformData());
        kPBHandle.Init(PBFLOATCHANNEL, m_usPBIndices[1]);
    }

    return kPBHandle;
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::BindInfo::MatchesIDTag(
    const NiEvaluator::IDTag& kIDTag) const
{
    if (m_kAVObjectName == kIDTag.GetAVObjectName() &&
        m_kPropertyType == kIDTag.GetPropertyType() &&
        m_kCtlrType == kIDTag.GetCtlrType() &&
        m_kCtlrID == kIDTag.GetCtlrID() &&
        m_kEvaluatorID == kIDTag.GetEvaluatorID())
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::BindInfo::MatchesIDTag(
    const NiFixedString& kAVObjectName, 
    const NiFixedString& kPropertyType, 
    const NiFixedString& kCtlrType, 
    const NiFixedString& kCtlrID, 
    const NiFixedString& kEvaluatorID) const
{
    if (m_kAVObjectName == kAVObjectName &&
        m_kPropertyType == kPropertyType &&
        m_kCtlrType == kCtlrType &&
        m_kCtlrID == kCtlrID &&
        m_kEvaluatorID == kEvaluatorID)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::BindInfo::IsTransformData() const
{
    return (m_kCtlrType == NiAnimationConstants::GetTransformCtlrType());
}
//---------------------------------------------------------------------------
inline void NiPoseBinding::BindInfo::AddPBHandle(NiPoseBufferHandle kPBHandle)
{
    if (IsTransformData())
    {
        // The pose buffer handle for position channels in transform 
        // evaluators is stored directly. The remaining two 
        // pose buffer handles are stored as indices to conserve memory.
        NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
        if (ePBChannelType == PBROTCHANNEL)
        {
            m_kPBHandle = kPBHandle;
        }
        else if (ePBChannelType == PBPOINT3CHANNEL)
        {
            m_usPBIndices[0] = kPBHandle.GetChannelIndex();
        }
        else
        {
            NIASSERT(ePBChannelType == PBFLOATCHANNEL);
            m_usPBIndices[1] = kPBHandle.GetChannelIndex();
        }
    }
    else
    {
        // Non-transform evaluators have just one pose buffer handle.
        m_kPBHandle = kPBHandle;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiPoseBinding
//---------------------------------------------------------------------------
inline NiPoseBinding::NiPoseBinding() : 
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
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::GetCumulativeAnimations() const
{
    return (m_usNonAccumStartIndex > 0);
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPoseBinding::GetAccumRootName() const
{
    return m_kAccumRootName;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNonAccumStartIndex() const
{
    return m_usNonAccumStartIndex;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumTotalBindings() const
{
    return m_usNumTotalBindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumColorBindings() const
{
    return m_usNumColorBindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumBoolBindings() const
{
    return m_usNumBoolBindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumFloatBindings() const
{
    return m_usNumFloatBindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumPoint3Bindings() const
{
    return m_usNumPoint3Bindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumRotBindings() const
{
    return m_usNumRotBindings;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumReferencedBindings() const
{
    return m_usNumReferencedBindings;
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::GetHandleAndLOD(
    const NiFixedString& kAVObjectName, const NiFixedString& kPropertyType, 
    const NiFixedString& kCtlrType, const NiFixedString& kCtlrID, 
    const NiFixedString& kEvaluatorID, 
    NiPoseBufferChannelType ePBChannelType, 
    NiPoseBufferHandle& kPBHandle, short& sLOD) const
{
    const BindInfo* pkBindInfo = GetBindInfo(kAVObjectName, kPropertyType,
        kCtlrType, kCtlrID, kEvaluatorID);
    if (pkBindInfo)
    {
        kPBHandle = pkBindInfo->GetPBHandle(ePBChannelType);
        if (kPBHandle.IsValid())
        {
            sLOD = pkBindInfo->m_sLOD;
            return true;
        }
    }

    kPBHandle.Invalidate();
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBinding::GetHandleAndLOD(
    const NiEvaluator::IDTag& kIDTag, 
    NiPoseBufferChannelType ePBChannelType, 
    NiPoseBufferHandle& kPBHandle, short& sLOD) const
{
    const BindInfo* pkBindInfo = GetBindInfo(kIDTag);
    if (pkBindInfo)
    {
        kPBHandle = pkBindInfo->GetPBHandle(ePBChannelType);
        if (kPBHandle.IsValid())
        {
            sLOD = pkBindInfo->m_sLOD;
            return true;
        }
    }

    kPBHandle.Invalidate();
    return false;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBinding::GetNumBindInfos() const
{
    return m_usNumBindInfos;
}
//---------------------------------------------------------------------------
inline NiPoseBinding::BindInfo* NiPoseBinding::GetBindInfos()
{
    return m_pkBindInfoList;
}
//---------------------------------------------------------------------------
inline unsigned short* NiPoseBinding::GetBindInfoIndexList()
{
    return m_pusBindInfoIndexList;
}
//---------------------------------------------------------------------------
