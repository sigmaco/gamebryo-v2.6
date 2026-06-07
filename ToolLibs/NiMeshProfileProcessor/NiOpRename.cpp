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

#include "NiOpRename.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpRename);

//---------------------------------------------------------------------------
bool NiOpRename::SetParameters(const NiFixedString& kSemantic, NiUInt8 uiIndex)
{
    m_kSemantic = kSemantic;
    m_uiIndex = uiIndex;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpRename::Resolve(NiMeshErrorInterface* pkReportedErrors)
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
    m_kOutStreamRef = 
        NiMeshProfileOperations::Rename(kStreamRef, m_kSemantic, m_uiIndex);

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(NiMPPMessages::ERROR_RENAME_FAILED);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiOpRename::GetOperationName()
{
    return "Rename";
}
//---------------------------------------------------------------------------
bool NiOpRename::GetSemantic(NiFixedString& kSemantic, 
    NiUInt8& uiSemanticIndex) const
{
    kSemantic = m_kSemantic;
    uiSemanticIndex = m_uiIndex;
    return true;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpRename::CopyMembers(NiOpRename* pkDest, 
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_kSemantic = m_kSemantic;
    pkDest->m_uiIndex = m_uiIndex;
}
//---------------------------------------------------------------------------
bool NiOpRename::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpRename* pkOtherNode = 
        NiDynamicCast(NiOpRename, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_kSemantic != m_kSemantic) 
        return false;
    if (pkOtherNode->m_uiIndex != m_uiIndex) 
        return false;

    return true;
}
//---------------------------------------------------------------------------

