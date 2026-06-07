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
// NSBD3D10UserDefinedData inline functions
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataBlock::NSBD3D10UserDefinedDataBlock(
    const char* pcName) :
    NSBD3D10ConstantMap(),
    m_pcName(0)
{
    SetName(pcName);
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataBlock::~NSBD3D10UserDefinedDataBlock()
{
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10UserDefinedDataBlock::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline void NSBD3D10UserDefinedDataBlock::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
