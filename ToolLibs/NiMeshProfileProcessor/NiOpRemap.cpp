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

#include "NiOpRemap.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpRemap);

//---------------------------------------------------------------------------
void NiOpRemap::SetParameters(NiInt32 iSelectedComponent, 
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd)
{
    m_iSelectedComponent = iSelectedComponent;
    m_dSrcBegin = dSrcBegin;
    m_dSrcEnd = dSrcEnd;
    m_dDstBegin = dDstBegin;
    m_dDstEnd = dDstEnd;
}
//---------------------------------------------------------------------------
bool NiOpRemap::Resolve(NiMeshErrorInterface* pkReportedErrors)
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
    m_kOutStreamRef = NiMeshProfileOperations::Remap(kStreamRef,
        m_dSrcBegin, m_dSrcEnd, m_dDstBegin, m_dDstEnd, m_iSelectedComponent);

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(NiMPPMessages::ERROR_REMAP_FAILED);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiOpRemap::GetOperationName()
{
    return "Remap";
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpRemap::CopyMembers(NiOpRemap* pkDest, NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_iSelectedComponent = m_iSelectedComponent;
    pkDest->m_dSrcBegin = m_dSrcBegin;
    pkDest->m_dSrcEnd = m_dSrcEnd;
    pkDest->m_dDstBegin = m_dDstBegin;
    pkDest->m_dDstEnd = m_dDstEnd;
}
//---------------------------------------------------------------------------
bool NiOpRemap::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpRemap* pkOtherNode = 
        NiDynamicCast(NiOpRemap, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_iSelectedComponent != m_iSelectedComponent)
        return false;
    if (pkOtherNode->m_dSrcBegin != m_dSrcBegin)
        return false;
    if (pkOtherNode->m_dSrcEnd != m_dSrcEnd) 
        return false;
    if (pkOtherNode->m_dDstBegin != m_dDstBegin) 
        return false;
    if (pkOtherNode->m_dDstEnd != m_dDstEnd)
        return false;

    return true;
}

//---------------------------------------------------------------------------

