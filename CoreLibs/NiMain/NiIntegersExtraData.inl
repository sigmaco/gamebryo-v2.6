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
//  NiIntegersExtraData inline functions

//---------------------------------------------------------------------------
inline NiIntegersExtraData::NiIntegersExtraData()
{
    m_piValue = NULL;
    m_uiSize = 0;
}
//---------------------------------------------------------------------------
inline NiIntegersExtraData::~NiIntegersExtraData()
{
    NiFree(m_piValue);
    m_piValue = NULL;
}
//---------------------------------------------------------------------------
inline void NiIntegersExtraData::GetArray(unsigned int &uiSize,
                                                int* &piValue) const
{
    uiSize = m_uiSize;
    piValue = m_piValue;
}
//---------------------------------------------------------------------------
