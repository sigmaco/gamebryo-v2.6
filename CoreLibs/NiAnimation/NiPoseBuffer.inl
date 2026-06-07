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
inline bool NiPoseBuffer::IsItemValid(unsigned int uiWeightAndFlags)
{
    return (uiWeightAndFlags & VALIDITEMFLAG) != 0;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsItemValidAndNotCulled(
    unsigned int uiWeightAndFlags)
{
    return ((uiWeightAndFlags & (VALIDITEMFLAG | CULLEDVALIDITEMFLAG)) == 
        VALIDITEMFLAG);
}
//---------------------------------------------------------------------------
inline float NiPoseBuffer::GetItemFinalizedWeight(
    unsigned int uiWeightAndFlags)
{
    NIASSERT(IsItemValidAndNotCulled(uiWeightAndFlags));
    const float FINALIZED_WEIGHT_INVERSE_SCALER = 
        1.0f / (float)FINALIZEDWEIGHTSCALER;
    return (float)(uiWeightAndFlags & FINALIZEDWEIGHTMASK) * 
        FINALIZED_WEIGHT_INVERSE_SCALER;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetItemFinalizedWeight(float fFinalizedWeight, 
    unsigned int& uiWeightAndFlags)
{
    NIASSERT(fFinalizedWeight >= 0.0f && fFinalizedWeight <= 1.0f);
    NIASSERT(IsItemValidAndNotCulled(uiWeightAndFlags));
    uiWeightAndFlags = VALIDITEMFLAG |
        (unsigned int)(fFinalizedWeight * (float)FINALIZEDWEIGHTSCALER);
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::NiPoseBuffer() : 
    m_spPoseBinding(NULL), m_usNumTotalItems(0), m_usNumValidItems(0),
    m_uiNonAccumStartIndex(0), m_puiFlagWeightArray(NULL), m_pkDataBlock(NULL),
    m_usNumColorItems(0), m_usColorStartIndex(0), m_pkColorItems(NULL),
    m_usNumBoolItems(0), m_usBoolStartIndex(0), m_pkBoolItems(NULL), 
    m_usNumFloatItems(0), m_usFloatStartIndex(0), m_pkFloatItems(NULL),
    m_usNumPoint3Items(0), m_usPoint3StartIndex(0), m_pkPoint3Items(NULL),
    m_usNumRotItems(0), m_usRotStartIndex(0), m_pkRotItems(NULL),
    m_usNumReferencedItems(0), m_usReferencedStartIndex(0), 
    m_pkReferencedItems(NULL)
{
}
//---------------------------------------------------------------------------
inline NiPoseBinding* NiPoseBuffer::GetPoseBinding() const
{
    return m_spPoseBinding;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumTotalItems() const
{
    return m_usNumTotalItems;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumValidItems() const
{
    return m_usNumValidItems;
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNonAccumStartIndex() const
{
    return m_uiNonAccumStartIndex;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsFullBodyPose() const
{
    NIASSERT(m_usNumValidItems <= m_usNumTotalItems);
    NIASSERT(m_spPoseBinding || m_usNumTotalItems == 0);
    NIASSERT(!m_spPoseBinding || 
        m_usNumTotalItems <= m_spPoseBinding->GetNumTotalBindings());
    return (!m_spPoseBinding || 
        m_usNumValidItems == m_spPoseBinding->GetNumTotalBindings());
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumColors() const
{
    return m_usNumColorItems;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetColor(NiPoseBufferHandle kPBHandle, 
    NiColorA& kColor) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumColorItems && IsItemValid(
        m_puiFlagWeightArray[m_usColorStartIndex + usIndex]))
    {
        kColor = m_pkColorItems[usIndex].m_kColor;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetColorIfNotCulled(NiPoseBufferHandle kPBHandle, 
    NiColorA& kColor) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumColorItems && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usColorStartIndex + usIndex]))
    {
        kColor = m_pkColorItems[usIndex].m_kColor;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsColorValid(NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumColorItems && IsItemValid(
        m_puiFlagWeightArray[m_usColorStartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetColor(NiPoseBufferHandle kPBHandle, 
    const NiColorA& kColor)
{
    NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumColorItems);
    m_pkColorItems[usIndex].m_kColor = kColor;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetColorValid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumColorItems);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usColorStartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumBools() const
{
    return m_usNumBoolItems;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetBool(NiPoseBufferHandle kPBHandle, 
    float& fBool) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumBoolItems && IsItemValid(
        m_puiFlagWeightArray[m_usBoolStartIndex + usIndex]))
    {
        fBool = m_pkBoolItems[usIndex].m_fBool;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsBoolValid(NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumBoolItems && IsItemValid(
        m_puiFlagWeightArray[m_usBoolStartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetBoolIfNotCulled(NiPoseBufferHandle kPBHandle, 
    float& fBool) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumBoolItems && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usBoolStartIndex + usIndex]))
    {
        fBool = m_pkBoolItems[usIndex].m_fBool;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetBool(NiPoseBufferHandle kPBHandle, float fBool)
{
    NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumBoolItems);
    m_pkBoolItems[usIndex].m_fBool = fBool;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetBoolValid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumBoolItems);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usBoolStartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumFloats() const
{
    return m_usNumFloatItems;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetFloat(NiPoseBufferHandle kPBHandle, 
    float& fFloat) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumFloatItems && IsItemValid(
        m_puiFlagWeightArray[m_usFloatStartIndex + usIndex]))
    {
        fFloat = m_pkFloatItems[usIndex].m_fFloat;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetFloatIfNotCulled(NiPoseBufferHandle kPBHandle, 
    float& fFloat) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumFloatItems && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usFloatStartIndex + usIndex]))
    {
        fFloat = m_pkFloatItems[usIndex].m_fFloat;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsFloatValid(NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumFloatItems && IsItemValid(
        m_puiFlagWeightArray[m_usFloatStartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetFloat(NiPoseBufferHandle kPBHandle, float fFloat)
{
    NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumFloatItems);
    m_pkFloatItems[usIndex].m_fFloat = fFloat;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetFloatValid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumFloatItems);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usFloatStartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumPoint3s() const
{
    return m_usNumPoint3Items;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetPoint3(NiPoseBufferHandle kPBHandle, 
    NiPoint3& kPoint3) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumPoint3Items && IsItemValid(
        m_puiFlagWeightArray[m_usPoint3StartIndex + usIndex]))
    {
        kPoint3 = m_pkPoint3Items[usIndex].m_kPoint3;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetPoint3IfNotCulled(NiPoseBufferHandle kPBHandle, 
    NiPoint3& kPoint3) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumPoint3Items && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usPoint3StartIndex + usIndex]))
    {
        kPoint3 = m_pkPoint3Items[usIndex].m_kPoint3;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsPoint3Valid(NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumPoint3Items && IsItemValid(
        m_puiFlagWeightArray[m_usPoint3StartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetPoint3(NiPoseBufferHandle kPBHandle, 
    const NiPoint3& kPoint3)
{
    NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumPoint3Items);
    m_pkPoint3Items[usIndex].m_kPoint3 = kPoint3;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetPoint3Valid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumPoint3Items);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usPoint3StartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumRots() const
{
    return m_usNumRotItems;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetRot(NiPoseBufferHandle kPBHandle, 
    NiQuaternion& kRot) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumRotItems && IsItemValid(
        m_puiFlagWeightArray[m_usRotStartIndex + usIndex]))
    {
        kRot = m_pkRotItems[usIndex].m_kRot;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetRotIfNotCulled(NiPoseBufferHandle kPBHandle, 
    NiQuaternion& kRot) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumRotItems && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usRotStartIndex + usIndex]))
    {
        kRot = m_pkRotItems[usIndex].m_kRot;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsRotValid(NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumRotItems && IsItemValid(
        m_puiFlagWeightArray[m_usRotStartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetRot(NiPoseBufferHandle kPBHandle, 
    const NiQuaternion& kRot)
{
    NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumRotItems);
    m_pkRotItems[usIndex].m_kRot = kRot;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetRotValid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumRotItems);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usRotStartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPoseBuffer::GetNumReferencedItems() const
{
    return m_usNumReferencedItems;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetReferencedItem(NiPoseBufferHandle kPBHandle, 
    ReferencedItem& kItem) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumReferencedItems && IsItemValid(
        m_puiFlagWeightArray[m_usReferencedStartIndex + usIndex]))
    {
        kItem = m_pkReferencedItems[usIndex];
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetReferencedItemIfNotCulled(
    NiPoseBufferHandle kPBHandle, ReferencedItem& kItem) const
{
    NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    if (usIndex < m_usNumReferencedItems && IsItemValidAndNotCulled(
        m_puiFlagWeightArray[m_usReferencedStartIndex + usIndex]))
    {
        kItem = m_pkReferencedItems[usIndex];
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::IsReferencedItemValid(NiPoseBufferHandle kPBHandle)
    const
{
    NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    return (usIndex < m_usNumReferencedItems && IsItemValid(
        m_puiFlagWeightArray[m_usReferencedStartIndex + usIndex]));
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetReferencedItem(NiPoseBufferHandle kPBHandle, 
    const ReferencedItem& kItem)
{
    NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumReferencedItems);
    m_pkReferencedItems[usIndex] = kItem;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::SetReferencedItemValid(NiPoseBufferHandle kPBHandle, 
    bool bValid)
{
    NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NIASSERT(usIndex < m_usNumReferencedItems);
    unsigned int& uiFlagWeight = m_puiFlagWeightArray[
        m_usReferencedStartIndex + usIndex];
    if (IsItemValid(uiFlagWeight))
    {
        NIASSERT(m_usNumValidItems > 0);
        m_usNumValidItems--;
    }
    if (bValid)
    {
        NIASSERT(m_usNumValidItems < m_usNumTotalItems);
        m_usNumValidItems++;
        // Overwrite flags and finalized weight
        uiFlagWeight = (unsigned int)VALIDITEMFLAG;
    }
    else
    {
        // Overwrite flags and finalized weight
        uiFlagWeight = 0;
    }
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetHandleAndLOD(const NiFixedString& kAVObjectName,
    const NiFixedString& kPropertyType, const NiFixedString& kCtlrType,
    const NiFixedString& kCtlrID, const NiFixedString& kInterpolatorID, 
    NiPoseBufferChannelType ePBChannelType, 
    NiPoseBufferHandle& kPBHandle, short& sLOD) const
{
    if (m_spPoseBinding)
    {
        return m_spPoseBinding->GetHandleAndLOD(kAVObjectName,
            kPropertyType, kCtlrType, kCtlrID, kInterpolatorID, 
            ePBChannelType, kPBHandle, sLOD);
    }
    kPBHandle.Invalidate();
    return false;
}
//---------------------------------------------------------------------------
inline bool NiPoseBuffer::GetHandleAndLOD(const NiEvaluator::IDTag& kIDTag, 
    NiPoseBufferChannelType ePBChannelType, 
    NiPoseBufferHandle& kPBHandle, short& sLOD) const
{
    if (m_spPoseBinding)
    {
        return m_spPoseBinding->GetHandleAndLOD(kIDTag, ePBChannelType, 
            kPBHandle, sLOD);
    }
    kPBHandle.Invalidate();
    return false;
}
//---------------------------------------------------------------------------
inline float* NiPoseBuffer::GetDataBlock()
{
    return m_pkDataBlock;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::ColorItem* NiPoseBuffer::GetColorDataBlock()
{
    return m_pkColorItems;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::BoolItem* NiPoseBuffer::GetBoolDataBlock()
{
    return m_pkBoolItems;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::FloatItem* NiPoseBuffer::GetFloatDataBlock()
{
    return m_pkFloatItems;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::Point3Item* NiPoseBuffer::GetPoint3DataBlock()
{
    return m_pkPoint3Items;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::RotItem* NiPoseBuffer::GetRotDataBlock()
{
    return m_pkRotItems;
}
//---------------------------------------------------------------------------
inline NiPoseBuffer::ReferencedItem* NiPoseBuffer::GetReferencedDataBlock()
{
    return m_pkReferencedItems;
}
//---------------------------------------------------------------------------
inline unsigned int* NiPoseBuffer::GetFlagWeightArray()
{
    return m_puiFlagWeightArray;
}
//---------------------------------------------------------------------------
inline void NiPoseBuffer::InvalidateAllItems()
{
    // This function quickly invalidates all data items in the pose buffer.
    if (m_usNumTotalItems > 0)
    {
        NIASSERT(m_puiFlagWeightArray);
        memset(m_puiFlagWeightArray, 0, 
            m_usNumTotalItems * sizeof(unsigned int));
        m_usNumValidItems = 0;
    }
}
//---------------------------------------------------------------------------
