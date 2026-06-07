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
//  NiBinaryExtraData inline functions

//---------------------------------------------------------------------------
inline NiBinaryExtraData::NiBinaryExtraData()
{
    m_uiSize = 0;
    m_pcBinaryData = NULL;
}
//---------------------------------------------------------------------------
inline NiBinaryExtraData::NiBinaryExtraData(const unsigned int uiSize,
                                            char* pcBinaryData)
{
    SetValue(uiSize, pcBinaryData);
}
//---------------------------------------------------------------------------
inline NiBinaryExtraData::~NiBinaryExtraData ()
{
    NiFree(m_pcBinaryData);
    m_pcBinaryData = NULL;
}
//---------------------------------------------------------------------------
inline void NiBinaryExtraData::GetValue(unsigned int &uiSize,
                                        char* &pcBinaryData) const
{
    uiSize = m_uiSize;
    pcBinaryData = m_pcBinaryData;
}
//---------------------------------------------------------------------------
inline void NiBinaryExtraData::SetValue(const unsigned int uiSize,
                                        char* pcBinaryData)
{
    m_uiSize = uiSize;
    m_pcBinaryData = pcBinaryData;
}
//---------------------------------------------------------------------------
