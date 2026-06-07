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
// NiEvaluator::IDTag
//---------------------------------------------------------------------------
inline NiEvaluator::IDTag::IDTag(
    const NiFixedString& kAVObjectName, const NiFixedString& kPropertyType,
    const NiFixedString& kCtlrType, const NiFixedString& kCtlrID,
    const NiFixedString& kEvaluatorID) 
{
    ClearValues();
    m_kAVObjectName = kAVObjectName;
    m_kPropertyType = kPropertyType;
    m_kCtlrType = kCtlrType;
    m_kCtlrID = kCtlrID;
    m_kEvaluatorID = kEvaluatorID;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline NiEvaluator::IDTag::IDTag(const IDTag& kOther)
{
    m_kAVObjectName = kOther.m_kAVObjectName;
    m_kPropertyType = kOther.m_kPropertyType;
    m_kCtlrType = kOther.m_kCtlrType;
    m_kCtlrID = kOther.m_kCtlrID;
    m_kEvaluatorID = kOther.m_kEvaluatorID;
    m_usSmallHashTableValue = kOther.m_usSmallHashTableValue;
    m_usLargeHashTableValue = kOther.m_usLargeHashTableValue;
}
//---------------------------------------------------------------------------
inline NiEvaluator::IDTag& 
NiEvaluator::IDTag::operator=(const IDTag& kTag)
{
    m_kAVObjectName = kTag.m_kAVObjectName;
    m_kPropertyType = kTag.m_kPropertyType;
    m_kCtlrType = kTag.m_kCtlrType;
    m_kCtlrID = kTag.m_kCtlrID;
    m_kEvaluatorID = kTag.m_kEvaluatorID;
    m_usSmallHashTableValue = kTag.m_usSmallHashTableValue;
    m_usLargeHashTableValue = kTag.m_usLargeHashTableValue;
    return *this;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::IDTag::GetAVObjectName() const
{
    return m_kAVObjectName;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::SetAVObjectName(
    const NiFixedString& kAVObjectName)
{
    m_kAVObjectName = kAVObjectName;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::IDTag::GetPropertyType() const
{
    return m_kPropertyType;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::SetPropertyType(
    const NiFixedString& kPropertyType)
{
    m_kPropertyType = kPropertyType;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::IDTag::GetCtlrType() const
{
    return m_kCtlrType;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::SetCtlrType(const NiFixedString& kCtlrType)
{
    m_kCtlrType = kCtlrType;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::IDTag::GetCtlrID() const
{
    return m_kCtlrID;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::SetCtlrID(const NiFixedString& kCtlrID)
{
    m_kCtlrID = kCtlrID;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::IDTag::GetEvaluatorID() const
{
    return m_kEvaluatorID;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::SetEvaluatorID(
    const NiFixedString& kEvaluatorID)
{
    m_kEvaluatorID = kEvaluatorID;
    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::ClearValues()
{
    m_kAVObjectName = NULL;
    m_kPropertyType = NULL;
    m_kCtlrType = NULL;
    m_kCtlrID = NULL;
    m_kEvaluatorID = NULL;
    NIASSERT(GetHashValue(m_kAVObjectName, m_kPropertyType, 
        m_kCtlrType, m_kCtlrID, m_kEvaluatorID) == 0);
    m_usSmallHashTableValue = 0;
    m_usLargeHashTableValue = 0;
}
//---------------------------------------------------------------------------
inline unsigned short NiEvaluator::IDTag::GetHashValue(
    const char* pcAVObjectName, const char* pcPropertyType, 
    const char* pcCtlrType, const char* pcCtlrID, 
    const char* pcEvaluatorID)
{
    unsigned int uiHash = 0;

    if (pcAVObjectName)
    {
        while (*pcAVObjectName)
            uiHash = (uiHash << 5) + uiHash + *pcAVObjectName++;
    }

    if (pcPropertyType)
    {
        while (*pcPropertyType)
            uiHash = (uiHash << 5) + uiHash + *pcPropertyType++;
    }

    if (pcCtlrType)
    {
        while (*pcCtlrType)
            uiHash = (uiHash << 5) + uiHash + *pcCtlrType++;
    }

    if (pcCtlrID)
    {
        while (*pcCtlrID)
            uiHash = (uiHash << 5) + uiHash + *pcCtlrID++;
    }

    if (pcEvaluatorID)
    {
        while (*pcEvaluatorID)
            uiHash = (uiHash << 5) + uiHash + *pcEvaluatorID++;
    }

    return (unsigned short)uiHash;
}
//---------------------------------------------------------------------------
inline unsigned short NiEvaluator::IDTag::GetSmallHashTableValue() const
{
    return m_usSmallHashTableValue;
}
//---------------------------------------------------------------------------
inline unsigned short NiEvaluator::IDTag::GetLargeHashTableValue() const
{
    return m_usLargeHashTableValue;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::IDTag::UpdateHashTableValues()
{
    unsigned short usHashValue = GetHashValue(m_kAVObjectName, 
        m_kPropertyType, m_kCtlrType, m_kCtlrID, m_kEvaluatorID);
    m_usSmallHashTableValue = (unsigned short)
        (usHashValue % SMALLHASHTABLESIZE);
    m_usLargeHashTableValue = (unsigned short)
        (usHashValue % LARGEHASHTABLESIZE);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiEvaluator
//---------------------------------------------------------------------------
inline void NiEvaluator::GetIDTag(const char*& pcAVObjectName, 
    const char*& pcPropertyType, const char*& pcCtlrType, 
    const char*& pcCtlrID, const char*& pcEvaluatorID) const
{
    pcAVObjectName = m_kIDTag.GetAVObjectName();
    pcPropertyType = m_kIDTag.GetPropertyType();
    pcCtlrType = m_kIDTag.GetCtlrType();
    pcCtlrID = m_kIDTag.GetCtlrID();
    pcEvaluatorID = m_kIDTag.GetEvaluatorID();
}
//---------------------------------------------------------------------------
inline void NiEvaluator::GetIDTag(NiFixedString& kAVObjectName, 
    NiFixedString& kPropertyType, NiFixedString& kCtlrType, 
    NiFixedString& kCtlrID, NiFixedString& kEvaluatorID) const
{
    kAVObjectName = m_kIDTag.GetAVObjectName();
    kPropertyType = m_kIDTag.GetPropertyType();
    kCtlrType = m_kIDTag.GetCtlrType();
    kCtlrID = m_kIDTag.GetCtlrID();
    kEvaluatorID = m_kIDTag.GetEvaluatorID();
}
//---------------------------------------------------------------------------
inline const NiEvaluator::IDTag& NiEvaluator::GetIDTag() const
{
    return  m_kIDTag;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::GetAVObjectName() const
{
    return m_kIDTag.GetAVObjectName();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::GetPropertyType() const
{
    return m_kIDTag.GetPropertyType();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::GetCtlrType() const
{
    return m_kIDTag.GetCtlrType();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::GetCtlrID() const
{
    return m_kIDTag.GetCtlrID();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEvaluator::GetEvaluatorID() const
{
    return m_kIDTag.GetEvaluatorID();
}
//---------------------------------------------------------------------------
inline void NiEvaluator::SetIDTag(const NiFixedString& kAVObjectName, 
    const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
    const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID)
{
    SetIDTag(IDTag(kAVObjectName, kPropertyType, kCtlrType, 
        kCtlrID, kEvaluatorID));
}
//---------------------------------------------------------------------------
inline void NiEvaluator::SetIDTag(const IDTag& kIDTag)
{
    m_kIDTag = kIDTag;
}
//---------------------------------------------------------------------------
inline unsigned int NiEvaluator::GetMaxEvalChannels() const
{
    // All evaluators have one channel except transform evaluators 
    // which may have up to three channels.
    NIASSERT(EVALTRANSFORMFLAG == 2);
    return (m_aiEvalChannelTypes[EVALFLAGINDEX] & EVALTRANSFORMFLAG) + 1;
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::IsEvalChannelInvalid(unsigned int uiChannel) const
{
    NIASSERT(uiChannel < GetMaxEvalChannels());
    return m_aiEvalChannelTypes[uiChannel] == EVALINVALIDCHANNEL;
}
//---------------------------------------------------------------------------
inline NiPoseBufferChannelType NiEvaluator::GetEvalPBChannelType(
    unsigned int uiChannel) const
{
    NIASSERT(uiChannel < GetMaxEvalChannels());

    NiPoseBufferChannelType ePBChannelType = PBREFERENCEDCHANNEL;
    if (!IsReferencedEvaluator())
    {
        NIASSERT(m_aiEvalChannelTypes[uiChannel] != 
            (EVALPOSEDFLAG | EVALINVALIDCHANNEL));
        if (m_aiEvalChannelTypes[uiChannel] != EVALINVALIDCHANNEL)
        {
            NIASSERT(PBCOLORCHANNEL == (1 << (EVALCOLORCHANNEL - 1)));
            NIASSERT(PBBOOLCHANNEL == (1 << (EVALBOOLCHANNEL - 1)));
            NIASSERT(PBFLOATCHANNEL == (1 << (EVALFLOATCHANNEL - 1)));
            NIASSERT(PBPOINT3CHANNEL == (1 << (EVALPOINT3CHANNEL - 1)));
            NIASSERT(PBROTCHANNEL == (1 << (EVALROTCHANNEL - 1)));
            ePBChannelType = (NiPoseBufferChannelType)(1 << 
                ((m_aiEvalChannelTypes[uiChannel] & ~EVALPOSEDFLAG) - 1));
        }
        else
        {
            ePBChannelType = PBINVALIDCHANNEL;
        }
    }

    NIASSERT(ePBChannelType != PBINVALIDCHANNEL || IsTransformEvaluator());

    return ePBChannelType;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::GetEvalPBChannelType(unsigned int uiChannel, 
    NiPoseBufferChannelType& ePBChannelType, bool& bPosed) const
{
    NIASSERT(uiChannel < GetMaxEvalChannels());
    if (!IsReferencedEvaluator())
    {
        NIASSERT(m_aiEvalChannelTypes[uiChannel] != 
            (EVALPOSEDFLAG | EVALINVALIDCHANNEL));
        if (m_aiEvalChannelTypes[uiChannel] != EVALINVALIDCHANNEL)
        {
            NIASSERT(PBCOLORCHANNEL == (1 << (EVALCOLORCHANNEL - 1)));
            NIASSERT(PBBOOLCHANNEL == (1 << (EVALBOOLCHANNEL - 1)));
            NIASSERT(PBFLOATCHANNEL == (1 << (EVALFLOATCHANNEL - 1)));
            NIASSERT(PBPOINT3CHANNEL == (1 << (EVALPOINT3CHANNEL - 1)));
            NIASSERT(PBROTCHANNEL == (1 << (EVALROTCHANNEL - 1)));
            ePBChannelType = (NiPoseBufferChannelType)(1 << 
                ((m_aiEvalChannelTypes[uiChannel] & ~EVALPOSEDFLAG) - 1));
            bPosed = (m_aiEvalChannelTypes[uiChannel] & 
                EVALPOSEDFLAG) != 0;
        }
        else
        {
            ePBChannelType = PBINVALIDCHANNEL;
            bPosed = false;
        }
    }
    else
    {
        // Referenced channels are always posed since the reference itself
        // never changes. Note this doesn't imply the evaluator has 
        // posed channel data (i.e. the channel data may change, but the 
        // reference does not).
        ePBChannelType = PBREFERENCEDCHANNEL;
        bPosed = true;
    }

    NIASSERT(ePBChannelType != PBINVALIDCHANNEL || IsTransformEvaluator());
}
//---------------------------------------------------------------------------
inline NiPoseBufferChannelType NiEvaluator::GetRawEvalPBChannelType(
    unsigned int uiChannel) const
{
    NIASSERT(uiChannel < GetMaxEvalChannels());

    NiPoseBufferChannelType ePBChannelType = PBINVALIDCHANNEL;

    NIASSERT(m_aiEvalChannelTypes[uiChannel] != 
        (EVALPOSEDFLAG | EVALINVALIDCHANNEL));
    if (m_aiEvalChannelTypes[uiChannel] != EVALINVALIDCHANNEL)
    {
        NIASSERT(PBCOLORCHANNEL == (1 << (EVALCOLORCHANNEL - 1)));
        NIASSERT(PBBOOLCHANNEL == (1 << (EVALBOOLCHANNEL - 1)));
        NIASSERT(PBFLOATCHANNEL == (1 << (EVALFLOATCHANNEL - 1)));
        NIASSERT(PBPOINT3CHANNEL == (1 << (EVALPOINT3CHANNEL - 1)));
        NIASSERT(PBROTCHANNEL == (1 << (EVALROTCHANNEL - 1)));
        ePBChannelType = (NiPoseBufferChannelType)(1 << 
            ((m_aiEvalChannelTypes[uiChannel] & ~EVALPOSEDFLAG) - 1));
    }

    NIASSERT(ePBChannelType != PBINVALIDCHANNEL || IsTransformEvaluator());

    return ePBChannelType;
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::IsRawEvalChannelPosed(unsigned int uiChannel) const
{
    NIASSERT(uiChannel < GetMaxEvalChannels());
    return (m_aiEvalChannelTypes[uiChannel] & EVALPOSEDFLAG) != 0;
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::AlwaysUpdate() const
{
    return (m_aiEvalChannelTypes[EVALFLAGINDEX] & EVALALWAYSUPDATEFLAG) != 0;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::SetAlwaysUpdate(bool bEnabled)
{
    if (bEnabled)
    {
        m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALALWAYSUPDATEFLAG;
    }
    else
    {
        m_aiEvalChannelTypes[EVALFLAGINDEX] &= ~EVALALWAYSUPDATEFLAG;
    }
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::IsTransformEvaluator() const
{
    return (m_aiEvalChannelTypes[EVALFLAGINDEX] & EVALTRANSFORMFLAG) != 0;
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::IsReferencedEvaluator() const
{
    return (m_aiEvalChannelTypes[EVALFLAGINDEX] & EVALREFERENCEDFLAG) != 0;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::SetReferencedEvaluator()
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALREFERENCEDFLAG;
}
//---------------------------------------------------------------------------
inline bool NiEvaluator::IsShutdownRequired() const
{
    return (m_aiEvalChannelTypes[EVALFLAGINDEX] & EVALSHUTDOWNFLAG) != 0;
}
//---------------------------------------------------------------------------
inline void NiEvaluator::SetEvalChannelPosed(unsigned int uiChannel)
{
    NIASSERT(uiChannel < GetMaxEvalChannels());
    m_aiEvalChannelTypes[uiChannel] |= EVALPOSEDFLAG;
}
//---------------------------------------------------------------------------
