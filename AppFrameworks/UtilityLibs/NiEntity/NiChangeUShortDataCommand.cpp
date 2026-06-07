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
#include "NiEntityPCH.h"

#include "NiChangeUShortDataCommand.h"

//---------------------------------------------------------------------------
NiChangeUShortDataCommand::NiChangeUShortDataCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName, unsigned short usData,
    unsigned int uiPropertyIndex) : NiChangePropertyCommand(
    pkEntityPropertyInterface, kPropertyName, uiPropertyIndex),
    m_usNewData(usData)
{
    NiFixedString kPrimitiveType;
    NIVERIFY(m_spEntityPropertyInterface->GetPrimitiveType(
        m_kPropertyName, kPrimitiveType));
    NIASSERT(kPrimitiveType == NiEntityPropertyInterface::PT_USHORT);
}
//---------------------------------------------------------------------------
// NiChangePropertyCommand overrides.
//---------------------------------------------------------------------------
NiBool NiChangeUShortDataCommand::StoreOldData()
{
    return m_spEntityPropertyInterface->GetPropertyData(
        m_kPropertyName, m_usOldData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
NiBool NiChangeUShortDataCommand::SetNewData()
{
    return m_spEntityPropertyInterface->SetPropertyData(
        m_kPropertyName, m_usNewData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
NiBool NiChangeUShortDataCommand::SetOldData()
{
    return m_spEntityPropertyInterface->SetPropertyData(
        m_kPropertyName, m_usOldData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
