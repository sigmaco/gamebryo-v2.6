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
//  NiStringsExtraData inline functions

//---------------------------------------------------------------------------
inline NiStringsExtraData::NiStringsExtraData()
{
    m_ppcValue = NULL;
    m_uiSize = 0;
}
//---------------------------------------------------------------------------
inline NiStringsExtraData::~NiStringsExtraData()
{
    for (unsigned int i=0; i < m_uiSize; i++)
    {
        NiFree(m_ppcValue[i]);
    }
    NiFree(m_ppcValue);
    m_ppcValue = NULL;
}
//---------------------------------------------------------------------------
inline void NiStringsExtraData::GetArray(unsigned int &uiSize,
                                               char** &ppcValue) const
{
    uiSize = m_uiSize;
    ppcValue = m_ppcValue;
}
//---------------------------------------------------------------------------
