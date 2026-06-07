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

/*
    MaxOrdering m_eOrdering;
    NiPoint2 m_kUVScale;
    NiPoint2 m_kUVOffset;
    NiPoint2 m_kUVCropOffset;
    NiPoint2 m_kUVCropDimensions;
    float m_fWAngle;
*/
//---------------------------------------------------------------------------
inline unsigned short* NiMAXUV::GetOrder(unsigned int uiMAXChannel)
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    return m_kChannels.GetAt(uiMAXChannel)->m_ausOrder;
}
//---------------------------------------------------------------------------

inline NiPoint2 NiMAXUV::GetTiling(unsigned int uiMAXChannel, Texmap* pkMap)
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    Channel* pkChannel = m_kChannels.GetAt(uiMAXChannel);

    NIASSERT(pkChannel);
    for (unsigned int ui = 0; ui < pkChannel->m_kTexmaps.GetSize(); ui++)
    {
        Channel::TexmapInfo* pkInfo = pkChannel->m_kTexmaps.GetAt(ui);
        if (pkInfo != NULL && pkInfo->m_pkTexmap == pkMap)
            return pkInfo->m_kUVScale;
    }
//    NIASSERT(!"Did not find tiling info!");
    return NiPoint2(1.0f, 1.0f);
}
//---------------------------------------------------------------------------

inline NiPoint2 NiMAXUV::GetOffset (unsigned int uiMAXChannel, Texmap* pkMap)
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    Channel* pkChannel = m_kChannels.GetAt(uiMAXChannel);

    NIASSERT(pkChannel);
    for (unsigned int ui = 0; ui < pkChannel->m_kTexmaps.GetSize(); ui++)
    {
        Channel::TexmapInfo* pkInfo = pkChannel->m_kTexmaps.GetAt(ui);
        if (pkInfo != NULL && pkInfo->m_pkTexmap == pkMap)
            return pkInfo->m_kUVOffset;
    }
 //   NIASSERT(!"Did not find offset info!");
    return NiPoint2(0.0f, 0.0f);}
//---------------------------------------------------------------------------

inline NiPoint2 NiMAXUV::GetCropOffset (unsigned int uiMAXChannel)
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    return m_kChannels.GetAt(uiMAXChannel)->m_kUVCropOffset;
}
//---------------------------------------------------------------------------

inline float NiMAXUV::GetWAngle( unsigned int uiMAXChannel , Texmap* pkMap)
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    Channel* pkChannel = m_kChannels.GetAt(uiMAXChannel);

    NIASSERT(pkChannel);
    for (unsigned int ui = 0; ui < pkChannel->m_kTexmaps.GetSize(); ui++)
    {
        Channel::TexmapInfo* pkInfo = pkChannel->m_kTexmaps.GetAt(ui);
        if (pkInfo != NULL && pkInfo->m_pkTexmap == pkMap)
            return pkInfo->m_fWAngle;
    }
    //NIASSERT(!"Did not find rotation info!");
    return 0.0f;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiMAXUV::GetCropDimensions( unsigned int uiMAXChannel )
{
    NIASSERT(m_kChannels.GetAt(uiMAXChannel));
    return m_kChannels.GetAt(uiMAXChannel)->m_kUVCropDimensions;
}
//---------------------------------------------------------------------------

inline bool NiMAXUV::GetUseVCForUV()
{
    return m_bUseVCForUV;
}
//---------------------------------------------------------------------------
inline void NiMAXUV::SetMAXtoNI(unsigned int uiMAXChannel, short sNIChannel)
{
    if (IsChannel(uiMAXChannel))
        m_kChannels.GetAt(uiMAXChannel)->m_sMAXtoNIchannel = sNIChannel;
}
//---------------------------------------------------------------------------
inline bool NiMAXUV::IsChannel(unsigned int uiIndex)
{
    if (uiIndex < m_kChannels.GetEffectiveSize() && 
        m_kChannels.GetAt(uiIndex))
        return (m_iAttrId == m_kChannels.GetAt(uiIndex)->m_iAttrId);
    else
        return false;
}
//---------------------------------------------------------------------------
inline bool NiMAXUV::Channel::IsEqual(NiMAXUV::Channel* pOther)
{
    if (m_kTexmaps.GetSize() != pOther->m_kTexmaps.GetSize())
        return false;

    for (unsigned int ui = 0; ui < m_kTexmaps.GetSize(); ui++)
    {
        Channel::TexmapInfo* pkInfo = m_kTexmaps.GetAt(ui);
        Channel::TexmapInfo* pkOtherInfo = m_kTexmaps.GetAt(ui);

        if (pkInfo == NULL && pkOtherInfo == NULL)
            continue;
        else if(pkInfo == NULL && pkOtherInfo != NULL)
            return false;
        else if(pkInfo != NULL && pkOtherInfo == NULL)
            return false;
        if (pkInfo->m_kUVScale == pkOtherInfo->m_kUVScale &&
            pkInfo->m_kUVOffset == pkOtherInfo->m_kUVOffset &&
            pkInfo->m_fWAngle == pkOtherInfo->m_fWAngle)
            continue;
        else
            return false;
    }

    return (
        m_ausOrder[0] == pOther->m_ausOrder[0] &&
        m_ausOrder[1] == pOther->m_ausOrder[1] &&        
        m_kUVCropOffset == pOther->m_kUVCropOffset &&
        m_kUVCropDimensions == pOther->m_kUVCropDimensions &&
        m_iMaxChannel == pOther->m_iMaxChannel && 
        m_iAttrId == pOther->m_iAttrId);
}


//---------------------------------------------------------------------------
inline bool NiMAXUV::Channel::CanShareChannelInfo(Channel* pOther)
{
    return (
        m_ausOrder[0] == pOther->m_ausOrder[0] &&
        m_ausOrder[1] == pOther->m_ausOrder[1] &&
        m_kUVCropOffset == pOther->m_kUVCropOffset &&
        m_kUVCropDimensions == pOther->m_kUVCropDimensions &&
        m_iMaxChannel == pOther->m_iMaxChannel && 
        m_iAttrId == pOther->m_iAttrId);
}
//---------------------------------------------------------------------------
inline int NiMAXUV::GetNumberOfChannels()
{
    return m_kChannels.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline int NiMAXUV::GetMAXChannelForNiChannelAt(unsigned int uiIndex)
{
    return (m_kChannels.GetAt(uiIndex))->m_iMaxChannel;
}
//---------------------------------------------------------------------------
inline int NiMAXUV::GetAttrId() const
{
    return m_iAttrId;
}
//---------------------------------------------------------------------------
inline void NiMAXUV::SetAttrId(int iAttrId)
{
    m_iAttrId = iAttrId;
}
//---------------------------------------------------------------------------

