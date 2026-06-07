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

#include "NiOpClamp.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpClamp);

//---------------------------------------------------------------------------
void NiOpClamp::SetParameters(NiInt32 iSelectedComponent, double dMin,
    double dMax)
{
    m_iSelectedComponent = iSelectedComponent;
    m_dMin = dMin;
    m_dMax = dMax;
}
//---------------------------------------------------------------------------
bool NiOpClamp::Resolve(NiMeshErrorInterface* pkReportedErrors)
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
    m_kOutStreamRef = NiMeshProfileOperations::Clamp(kStreamRef,
        m_iSelectedComponent, m_dMin, m_dMax);

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(NiMPPMessages::ERROR_CLAMP_FAILED);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiOpClamp::GetOperationName()
{
    return "Clamp";
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpClamp::CopyMembers(NiOpClamp* pkDest, NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_iSelectedComponent = m_iSelectedComponent;
    pkDest->m_dMin = m_dMin;
    pkDest->m_dMax = m_dMax;
}
//---------------------------------------------------------------------------
bool NiOpClamp::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpClamp* pkOtherNode = 
        NiDynamicCast(NiOpClamp, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_iSelectedComponent != m_iSelectedComponent) 
        return false;
    if (pkOtherNode->m_dMin != m_dMin)
        return false;
    if (pkOtherNode->m_dMax != m_dMax)
        return false;

    return true;
}
//---------------------------------------------------------------------------
