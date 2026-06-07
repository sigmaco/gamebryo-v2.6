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

#include "NiChangeUIntDataCommand.h"

//---------------------------------------------------------------------------
NiChangeUIntDataCommand::NiChangeUIntDataCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName, unsigned int uiData,
    unsigned int uiPropertyIndex) : NiChangePropertyCommand(
    pkEntityPropertyInterface, kPropertyName, uiPropertyIndex),
    m_uiNewData(uiData)
{
    NiFixedString kPrimitiveType;
    NIVERIFY(m_spEntityPropertyInterface->GetPrimitiveType(
        m_kPropertyName, kPrimitiveType));
    NIASSERT(kPrimitiveType == NiEntityPropertyInterface::PT_UINT);
}
//---------------------------------------------------------------------------
// NiChangePropertyCommand overrides.
//---------------------------------------------------------------------------
NiBool NiChangeUIntDataCommand::StoreOldData()
{
    return m_spEntityPropertyInterface->GetPropertyData(m_kPropertyName,
        m_uiOldData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
NiBool NiChangeUIntDataCommand::SetNewData()
{
    return m_spEntityPropertyInterface->SetPropertyData(m_kPropertyName,
        m_uiNewData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
NiBool NiChangeUIntDataCommand::SetOldData()
{
    return m_spEntityPropertyInterface->SetPropertyData(m_kPropertyName,
        m_uiOldData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
