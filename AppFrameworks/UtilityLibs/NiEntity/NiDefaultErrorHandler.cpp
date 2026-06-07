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

#include "NiDefaultErrorHandler.h"

//---------------------------------------------------------------------------
NiDefaultErrorHandler::NiDefaultErrorHandler(unsigned int uiErrorArraySize) :
    m_kErrors(uiErrorArraySize)
{
}
//---------------------------------------------------------------------------
void NiDefaultErrorHandler::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiDefaultErrorHandler::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
void NiDefaultErrorHandler::ReportError(const NiFixedString& kErrorMessage,
    const NiFixedString& kErrorDescription, const NiFixedString& kEntityName,
    const NiFixedString& kPropertyName)
{
    m_kErrors.Add(NiNew Error(kErrorMessage, kErrorDescription, kEntityName,
        kPropertyName));
}
//---------------------------------------------------------------------------
unsigned int NiDefaultErrorHandler::GetErrorCount() const
{
    return m_kErrors.GetSize();
}
//---------------------------------------------------------------------------
const NiFixedString& NiDefaultErrorHandler::GetErrorMessage(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kErrors.GetSize());
    return m_kErrors.GetAt(uiIndex)->m_kMessage;
}
//---------------------------------------------------------------------------
const NiFixedString& NiDefaultErrorHandler::GetErrorDescription(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kErrors.GetSize());
    return m_kErrors.GetAt(uiIndex)->m_kDescription;
}
//---------------------------------------------------------------------------
const NiFixedString& NiDefaultErrorHandler::GetEntityName(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kErrors.GetSize());
    return m_kErrors.GetAt(uiIndex)->m_kEntityName;
}
//---------------------------------------------------------------------------
const NiFixedString& NiDefaultErrorHandler::GetPropertyName(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kErrors.GetSize());
    return m_kErrors.GetAt(uiIndex)->m_kPropertyName;
}
//---------------------------------------------------------------------------
void NiDefaultErrorHandler::ClearErrors()
{
    m_kErrors.RemoveAll();
}
//---------------------------------------------------------------------------
