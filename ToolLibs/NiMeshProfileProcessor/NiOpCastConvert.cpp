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

#include "NiOpCastConvert.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpCastConvert);

//---------------------------------------------------------------------------
NiOpCastConvert::NiOpCastConvert() :
    m_eDstType(NiDataStreamElement::T_UINT32),
    m_eConvertType(NiMeshProfileOperations::CT_TYPECLAMP)
{
}
//---------------------------------------------------------------------------
bool NiOpCastConvert::SetParameters(NiDataStreamElement::Type eDstType,
    NiMeshProfileOperations::CastConvertType eConvertType)
{
    m_eDstType = eDstType;
    m_eConvertType = eConvertType;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpCastConvert::Resolve(NiMeshErrorInterface* pkReportedErrors)
{
    if (m_kOutStreamRef.IsValid())
        return true;

    // Can not resolve until an input operation is given.
    if (!m_spInputOp)
        return false;
    
    if (m_spInputOp->Resolve(pkReportedErrors) == false)
        return false;

    NiDataStreamRef& kStreamRef = m_spInputOp->GetOutput();
    NIASSERT(!m_kOutStreamRef.IsValid());
    m_kOutStreamRef = NiMeshProfileOperations::CastConvert(kStreamRef,
        m_eDstType, m_eConvertType);

    if (!m_kOutStreamRef.IsValid())
    {
        if (kStreamRef.IsValid() && kStreamRef.GetElementDescCount() > 0)
        {
            const NiFixedString& kSemantic = kStreamRef.GetSemanticNameAt(0);
            NiUInt8 uiSemanticIndex = (NiUInt8)kStreamRef.GetSemanticIndexAt(0);

            pkReportedErrors->ReportError(
                NiMPPMessages::ERROR_CASTCONVERT_FAILED_2ARGS,
                kSemantic, uiSemanticIndex);
        }

        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOpCastConvert::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    if (!m_spInputOp)
        return false;

    if (!m_spInputOp->RetrieveExpectedFinalFormat(eFormat))
        return false;

    if (NiDataStreamElement::IsSwizzled(eFormat) == true)
        return false;
    if (NiDataStreamElement::IsPacked(eFormat) == true)
        return false;

    // From the perspective of the NiOpCastConvert, the types are changed on
    // any inputs. Normalization may also change. No changes, however,
    // occur on the component count.
    NiUInt8 uiCompCnt = NiDataStreamElement::GetComponentCount(eFormat);

    NiDataStreamElement::Format eNewFormat =
        NiDataStreamElement::F_UNKNOWN;

    if (GetConvertType() == NiMeshProfileOperations::CT_NORMALIZE)
    {
        eNewFormat = NiDataStreamElement::GetPredefinedFormat(GetFormatType(),
            uiCompCnt, true);
    }
    else
    {
        eNewFormat = NiDataStreamElement::GetPredefinedFormat(GetFormatType(),
            uiCompCnt, false);
    }

    NIASSERT(eNewFormat != NiDataStreamElement::F_UNKNOWN);
    eFormat = eNewFormat;
    return true;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpCastConvert::RetrieveFinalFormatAlteringNode()
{
    return this;
}
//---------------------------------------------------------------------------
NiFixedString NiOpCastConvert::GetOperationName()
{
    return "CastConvert";
}
//---------------------------------------------------------------------------
NiDataStreamElement::Type NiOpCastConvert::GetFormatType() const
{
    return m_eDstType;
}
//---------------------------------------------------------------------------
NiMeshProfileOperations::CastConvertType 
    NiOpCastConvert::GetConvertType() const
{
    return m_eConvertType;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpCastConvert::CopyMembers(NiOpCastConvert* pkDest, 
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_eDstType = m_eDstType;
    pkDest->m_eConvertType = m_eConvertType;
}
//---------------------------------------------------------------------------
bool NiOpCastConvert::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpCastConvert* pkOtherNode = 
        NiDynamicCast(NiOpCastConvert, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_eDstType != m_eDstType) 
        return false;
    if (pkOtherNode->m_eConvertType != m_eConvertType)
        return false;
    return true;
}
//---------------------------------------------------------------------------

