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
inline NiPoseBufferHandle::NiPoseBufferHandle(
    NiPoseBufferChannelType ePBChannelType, unsigned short usChannelIndex) :
    m_usPBChannelType((unsigned short)ePBChannelType),
    m_usChannelIndex(usChannelIndex)
{
}
//---------------------------------------------------------------------------
inline NiPoseBufferHandle::NiPoseBufferHandle() :
    m_usPBChannelType(PBINVALIDCHANNEL), m_usChannelIndex(0)
{
}
//---------------------------------------------------------------------------
inline void NiPoseBufferHandle::Init(NiPoseBufferChannelType ePBChannelType, 
    unsigned short usChannelIndex)
{
    m_usPBChannelType = (unsigned short)ePBChannelType;
    m_usChannelIndex = usChannelIndex;
}
//---------------------------------------------------------------------------
inline void NiPoseBufferHandle::Invalidate()
{
    m_usPBChannelType = PBINVALIDCHANNEL;
    m_usChannelIndex = 0;
}
//---------------------------------------------------------------------------
inline bool NiPoseBufferHandle::IsValid() const
{
    return m_usPBChannelType != PBINVALIDCHANNEL;
}
//---------------------------------------------------------------------------
inline NiPoseBufferChannelType NiPoseBufferHandle::GetChannelType() const
{
    NIASSERT(IsValid());
    return (NiPoseBufferChannelType)m_usPBChannelType;
}
//---------------------------------------------------------------------------
inline unsigned short NiPoseBufferHandle::GetChannelIndex() const
{
    NIASSERT(IsValid());
    return m_usChannelIndex;
}
//---------------------------------------------------------------------------
inline bool NiPoseBufferHandle::operator== (const NiPoseBufferHandle& kOther) 
    const
{
    return (m_usPBChannelType == kOther.m_usPBChannelType && 
        m_usChannelIndex == kOther.m_usChannelIndex);
}
//---------------------------------------------------------------------------
inline bool NiPoseBufferHandle::operator!= (const NiPoseBufferHandle& kOther) 
    const
{
    return (m_usPBChannelType != kOther.m_usPBChannelType || 
        m_usChannelIndex != kOther.m_usChannelIndex);
}
//---------------------------------------------------------------------------
inline NiPoseBufferChannelType 
    NiPoseBufferHandle::GetChannelTypeNoValidation() const
{
    // This function returns the channel type without requiring a valid handle.
    // PBINVALIDCHANNEL is returned for invalid pose buffer handles.
    return (NiPoseBufferChannelType)m_usPBChannelType;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
inline void NiPoseBufferHandle::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream, m_usPBChannelType);
    NiStreamLoadBinary(stream, m_usChannelIndex);
}
//---------------------------------------------------------------------------
inline void NiPoseBufferHandle::SaveBinary(NiStream& stream) const
{
    NiStreamSaveBinary(stream, m_usPBChannelType);
    NiStreamSaveBinary(stream, m_usChannelIndex);
}
//---------------------------------------------------------------------------
