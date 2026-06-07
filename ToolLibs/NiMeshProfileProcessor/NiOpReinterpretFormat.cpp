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

#include "NiOpReinterpretFormat.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpReinterpretFormat);

//---------------------------------------------------------------------------
bool NiOpReinterpretFormat::Resolve(NiMeshErrorInterface* pkReportedErrors)
{
    if (m_kOutStreamRef.IsValid())
        return true;

    // Can not resolve until an input operation is given.
    if (!m_spInputOp)
        return false;
    
    if (m_spInputOp->Resolve(pkReportedErrors) == false)
        return false;

    NiDataStreamRef kStreamRef = m_spInputOp->GetOutput();
    NIASSERT(!m_kOutStreamRef.IsValid());
    
    m_kOutStreamRef = NiMeshProfileOperations::ReinterpretFormat(
        kStreamRef, m_eDstFormat);

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(
            NiMPPMessages::ERROR_REINTERPRETFORMAT_FAILED);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiOpReinterpretFormat::SetParameters(NiDataStreamElement::Format
    eDstFormat)
{
    m_eDstFormat = eDstFormat;
}
//---------------------------------------------------------------------------
bool NiOpReinterpretFormat::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    // From the perspective of the NiOpReinterpretFormat, it doesn't matter
    // if the inputs have been set yet or not. It doesn't even matter what the
    // possible formats of preceding operations are because this operation
    // reinterprets the format altogether.
    eFormat = m_eDstFormat;
    return true;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpReinterpretFormat::RetrieveFinalFormatAlteringNode()
{
    return this;
}
//---------------------------------------------------------------------------
NiFixedString NiOpReinterpretFormat::GetOperationName()
{
    return "ReinterpretFormat";
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpReinterpretFormat::CopyMembers(NiOpReinterpretFormat* pkDest, 
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_eDstFormat = m_eDstFormat;
}
//---------------------------------------------------------------------------
bool NiOpReinterpretFormat::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpReinterpretFormat* pkOtherNode = 
        NiDynamicCast(NiOpReinterpretFormat, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_eDstFormat != m_eDstFormat) 
        return false;

    return true;
}
//---------------------------------------------------------------------------
