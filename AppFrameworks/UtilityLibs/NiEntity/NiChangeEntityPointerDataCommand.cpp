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

#include "NiChangeEntityPointerDataCommand.h"

//---------------------------------------------------------------------------
NiChangeEntityPointerDataCommand::NiChangeEntityPointerDataCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName, NiEntityInterface* pkData,
    unsigned int uiPropertyIndex) : NiChangePropertyCommand(
    pkEntityPropertyInterface, kPropertyName, uiPropertyIndex),
    m_spNewData(pkData)
{
    NiFixedString kPrimitiveType;
    NIVERIFY(m_spEntityPropertyInterface->GetPrimitiveType(
        m_kPropertyName, kPrimitiveType));
    NIASSERT(kPrimitiveType == NiEntityPropertyInterface::PT_ENTITYPOINTER);
}
//---------------------------------------------------------------------------
// NiChangePropertyCommand overrides.
//---------------------------------------------------------------------------
NiBool NiChangeEntityPointerDataCommand::StoreOldData()
{
    NiEntityInterface* pkOldData;
    NiBool bSuccess = m_spEntityPropertyInterface->GetPropertyData(
        m_kPropertyName, pkOldData, m_uiPropertyIndex);
    if (bSuccess)
    {
        m_spOldData = pkOldData;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiChangeEntityPointerDataCommand::SetNewData()
{
    return m_spEntityPropertyInterface->SetPropertyData(m_kPropertyName,
        m_spNewData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
NiBool NiChangeEntityPointerDataCommand::SetOldData()
{
    return m_spEntityPropertyInterface->SetPropertyData(m_kPropertyName,
        m_spOldData, m_uiPropertyIndex);
}
//---------------------------------------------------------------------------
