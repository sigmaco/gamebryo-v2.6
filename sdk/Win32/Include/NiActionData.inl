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
// NiActionData inline functions
//---------------------------------------------------------------------------
inline NiActionData::NiActionData() :
    m_uiAppData(0), 
    m_uiDeviceID(0xffffffff), 
    m_uiDataValue(0), 
    m_pvContext(0), 
    m_uiTimeStamp(0), 
    m_uiSequence(0)
{
}
//---------------------------------------------------------------------------
inline NiActionData::~NiActionData()
{
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionData::GetAppData() const
{
    return m_uiAppData;
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionData::GetDeviceID() const
{
    return m_uiDeviceID;
}
//---------------------------------------------------------------------------
inline const void NiActionData::GetDeviceID(unsigned int& uiPort, 
    unsigned int& uiSlot) const
{
    uiPort = m_uiDeviceID & 0x0000ffff;
    uiSlot = (m_uiDeviceID & 0xffff0000) >> 16;
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionData::GetDataValue() const
{
    return m_uiDataValue;
}
//---------------------------------------------------------------------------
inline const void* NiActionData::GetContext() const
{
    return m_pvContext;
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionData::GetTimeStamp() const
{
    return m_uiTimeStamp;
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionData::GetSequence() const
{
    return m_uiSequence;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetAppData(unsigned int uiValue)
{
    m_uiAppData = uiValue;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetDeviceID(unsigned int uiValue)
{
    m_uiDeviceID = uiValue;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetDeviceID(unsigned int uiPort, 
    unsigned int uiSlot)
{
    m_uiDeviceID = uiPort | (uiSlot << 16);
}
//---------------------------------------------------------------------------
inline void NiActionData::SetDataValue(unsigned int uiValue)
{
    m_uiDataValue = uiValue;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetContext(void* pvContext)
{
    m_pvContext = pvContext;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetTimeStamp(unsigned int uiValue)
{
    m_uiTimeStamp = uiValue;
}
//---------------------------------------------------------------------------
inline void NiActionData::SetSequence(unsigned int uiValue)
{
    m_uiSequence = uiValue;
}
//---------------------------------------------------------------------------
