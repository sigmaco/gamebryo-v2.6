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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiMaterialDescriptor.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiMaterialDescriptor::NiMaterialDescriptor()
{
    for (unsigned int ui = 0; ui < BIT_ARRAY_SIZE; ui++)
    {
        m_auiBitArray[ui] = 0;
    }
}
//---------------------------------------------------------------------------
bool NiMaterialDescriptor::GenerateKey(char* pcValue,
    unsigned int uiMaxSize)
{
    NIVERIFY((unsigned int)NiSprintf(pcValue, uiMaxSize,
        "%.8X-%.8X-%.8X-%.8X-%.8X", 
        m_auiBitArray[0], m_auiBitArray[1], m_auiBitArray[2], 
        m_auiBitArray[3], m_auiBitArray[4]) <= uiMaxSize);
    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialDescriptor::IsEqual(
    const NiMaterialDescriptor* pkOther) const
{
    if (m_kIdentifier != pkOther->m_kIdentifier)
        return false;

    for (unsigned int ui = 0; ui < BIT_ARRAY_SIZE; ui++)
    {
        if (pkOther->m_auiBitArray[ui] != m_auiBitArray[ui])
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiMaterialDescriptor& NiMaterialDescriptor::operator=(
    const NiMaterialDescriptor& kDescriptor)
{
    m_kIdentifier = kDescriptor.m_kIdentifier;

    for (unsigned int ui = 0; ui < BIT_ARRAY_SIZE; ui++)
        m_auiBitArray[ui] = kDescriptor.m_auiBitArray[ui];

    return *this;
}
//---------------------------------------------------------------------------
