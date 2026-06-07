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

#include "NiOpComponentPack.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpComponentPack);

//---------------------------------------------------------------------------
void NiOpComponentPack::SetParameters(NiDataStreamElement::Format ePackFormat)
{
    NIASSERT(NiDataStreamElement::IsPacked(ePackFormat));
    m_ePackFormat = ePackFormat;
}
//---------------------------------------------------------------------------
bool NiOpComponentPack::Resolve(NiMeshErrorInterface* pkReportedErrors)
{
    if (m_kOutStreamRef.IsValid())
        return true;

    // Can not resolve until an input operation is given.
    if (!m_spInputOp)
        return false;
    
    if (m_spInputOp->Resolve(pkReportedErrors) == false)
        return false;

    NiDataStreamRef kStreamRef = m_spInputOp->GetOutput();
    NIASSERT(m_kOutStreamRef.IsValid() == false);

    if (NiDataStreamElement::IsPacked(m_ePackFormat) == false)
        return false;

    bool bNormalized = NiDataStreamElement::IsNormalized(m_ePackFormat);
    bool bSigned = NiDataStreamElement::IsSigned(m_ePackFormat);

    bool bLastFieldIsLiteral = 
        NiDataStreamElement::IsLastFieldLiteral(m_ePackFormat);

    NiUInt8 uiFieldCount = NiDataStreamElement::GetFieldCount(m_ePackFormat);
    NiUInt8 uiF[4];
    uiF[0] = uiF[1] = uiF[2] = uiF[3] = 0;

    for(NiUInt32 ui=0; ui<uiFieldCount; ui++)
        uiF[ui] = NiDataStreamElement::GetFieldBits(m_ePackFormat, (NiUInt8)ui);

    m_kOutStreamRef = NiMeshProfileOperations::ComponentPack(kStreamRef,
        bSigned, bNormalized, bLastFieldIsLiteral, uiF[0], uiF[1], uiF[2],
        uiF[3]);

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(NiMPPMessages::
            ERROR_COMPONENTPACK_FAILED);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOpComponentPack::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    if (!m_spInputOp)
        return false;

    eFormat = m_ePackFormat;
    return true;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpComponentPack::RetrieveFinalFormatAlteringNode()
{
    return this;
}
//---------------------------------------------------------------------------
NiFixedString NiOpComponentPack::GetOperationName()
{
    return "ComponentPack";
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpComponentPack::CopyMembers(NiOpComponentPack* pkDest,
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_ePackFormat = m_ePackFormat;
}
//---------------------------------------------------------------------------
bool NiOpComponentPack::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpComponentPack* pkOtherNode = 
        NiDynamicCast(NiOpComponentPack, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_ePackFormat != m_ePackFormat) 
        return false;

    return true;
}
//---------------------------------------------------------------------------
