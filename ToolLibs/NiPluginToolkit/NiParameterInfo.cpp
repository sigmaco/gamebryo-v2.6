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

#include "NiParameterInfo.h"

//---------------------------------------------------------------------------
NiParameterInfo::NiParameterInfo()
{
}
//---------------------------------------------------------------------------
const NiString& NiParameterInfo::GetKey() const
{
    return m_strKey;
}
//---------------------------------------------------------------------------
const NiString& NiParameterInfo::GetValue() const
{
    return m_strValue;
}
//---------------------------------------------------------------------------
/// Set the key string
void NiParameterInfo::SetKey(const char* pcKey)
{
    m_strKey = pcKey;
}

//---------------------------------------------------------------------------
/// Set the value string
void NiParameterInfo::SetValue(const char* pcValue)
{
    m_strValue = pcValue;
}
//---------------------------------------------------------------------------
