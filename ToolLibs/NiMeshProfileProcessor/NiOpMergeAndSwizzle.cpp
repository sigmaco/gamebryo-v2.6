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

#include "NiOpMergeAndSwizzle.h"
#include "NiComponentLiteralInput.h"
#include "NiComponentStreamInput.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpMergeAndSwizzle);

//---------------------------------------------------------------------------
NiOpMergeAndSwizzle::NiOpMergeAndSwizzle()
{
    m_uiExpectedInputs = 1;
    
    for(NiUInt32 ui=0; ui < 4; ui++)
    {
        m_auiComps[ui] = 0;
        m_adLiterals[ui] = 0.0;
        m_bLiteral[ui] = false;
        m_apInOps[ui] = NULL;
    }
}
//---------------------------------------------------------------------------
NiOpMergeAndSwizzle::~NiOpMergeAndSwizzle()
{
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::SetParameters(const NiFixedString& kSemantic,
    NiUInt8 uiIndex)
{
    m_kSemantic = kSemantic;
    m_uiIndex = uiIndex;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::Resolve(NiMeshErrorInterface* pkReportedErrors)
{
    // Make sure that we do not need to generate this stream
    for(NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_bLiteral[ui])
            continue;

        if (m_apInOps[ui] == NULL)
        {
            NIASSERT(!"Could not find expected input.");
            return false;
        }

        // Make sure we resolve and if we can generate
        if ((m_apInOps[ui]->Resolve(pkReportedErrors) == false))
        {
            NiFixedString kSemantic;
            NiUInt8 uiSemanticIndex;
            if (m_apInOps[ui]->GetSemantic(kSemantic, uiSemanticIndex))
            {
                pkReportedErrors->ReportError(
                    NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED_2ARGS,
                    kSemantic, uiSemanticIndex);
            }
            else
            {
                pkReportedErrors->ReportError(
                    NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED);
            }
            return false;
        }
    }

    // Execute Swizzle and Merge Operation
    NiComponentInput* apInputs[4];
    bool bError = false;

    NiUInt32 uiNullInputs = 0;
    for (NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        // If we have an output stream always use it.
        if (m_apInOps[ui] && m_apInOps[ui]->GetOutput().IsValid())
        {
            NiComponentStreamInput* pkStream =
                NiNew NiComponentStreamInput;
            if (pkStream->SetInput(m_apInOps[ui]->GetOutput(),
                m_auiComps[ui]) == false)
            {
                bError = true;
                NiDataStreamRef kSR =
                    m_apInOps[ui]->GetOutput();
                if (kSR.IsValid())
                {
                    const NiFixedString& kSemantic = kSR.GetSemanticNameAt(0);
                    NiUInt8 uiSemanticIndex = (NiUInt8)kSR.GetSemanticIndexAt(0);

                    pkReportedErrors->ReportError(NiMPPMessages::
                        ERROR_MERGEANDSWIZZLE_FAILED_TO_SET_3ARGS,
                        m_auiComps[ui], kSemantic, uiSemanticIndex);
                }
            }

            apInputs[ui] = pkStream;
        }
        else if (m_bLiteral[ui])
        {
            NiComponentLiteralInput* pkLiteral = 
                NiNew NiComponentLiteralInput();
            if (pkLiteral->SetInput(m_adLiterals[ui]) == false)
                bError = true;

            // Setting input for Literals should never fail
            NIASSERT(bError == false);
            apInputs[ui] = pkLiteral;
        }
        else
        {
            // This is an indication of an optional semantic not resolving.
            apInputs[ui] = NULL;
            uiNullInputs++;
        }
    }

    if (uiNullInputs == m_uiExpectedInputs)
    {
        // If all inputs are NULL, this is might be an indication that
        // none of the inputs were required. But regardless, no stream
        // can be produced, so resolve fails.
        return false;
    }

    if (bError == false)
    {
        switch(m_uiExpectedInputs)
        {
        case 1:
            m_kOutStreamRef =  NiMeshProfileOperations::MergeAndSwizzle(
                m_kSemantic, m_uiIndex, apInputs[0]);
            break;
        case 2:
            m_kOutStreamRef =  NiMeshProfileOperations::MergeAndSwizzle(
                m_kSemantic, m_uiIndex, apInputs[0], apInputs[1]);
            break;
        case 3:
            m_kOutStreamRef =  NiMeshProfileOperations::MergeAndSwizzle(
                m_kSemantic, m_uiIndex, apInputs[0], apInputs[1], apInputs[2]);
            break;
        case 4:
            m_kOutStreamRef =  NiMeshProfileOperations::MergeAndSwizzle(
                m_kSemantic, m_uiIndex, apInputs[0], apInputs[1], apInputs[2], 
                apInputs[3]);
            break;
        default:
            NIASSERT(!"Unsupported component number");
            break;
        }
    }

    for (NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        NiDelete apInputs[ui];
    }

    if (!m_kOutStreamRef.IsValid())
    {
        pkReportedErrors->ReportError(NiMPPMessages::
            ERROR_MERGEANDSWIZZLE_FAILED_UNKNOWN);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::SetExpectedInputs(NiUInt8 uiExpectedInputs)
{
    if (uiExpectedInputs < 1 || uiExpectedInputs > 4)
        return false;

    m_uiExpectedInputs = uiExpectedInputs;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::SetInput(NiOpNode*)
{
    // Overloaded to return false.
    return false;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::SetInput(NiUInt8 uiInput, NiOpNode* pkInOp,
    NiUInt8 uiComp, bool)
{
    if (uiInput >= m_uiExpectedInputs)
        return false;

    if (uiComp >= 4)
        return false;

    m_apInOps[uiInput] = pkInOp;
    m_auiComps[uiInput] = uiComp;
    m_bLiteral[uiInput] = false;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::SetInput(NiUInt8 uiInput, double dValue)
{
    if (uiInput >= m_uiExpectedInputs)
        return false;

    m_adLiterals[uiInput] = dValue;
    m_bLiteral[uiInput] = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::RetrieveExpectedFinalAccessAndUsage(NiUInt8&
    uiAccessFlags, NiDataStream::Usage& eUsage) const
{
    // Access and usage from first required data stream
    for(NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_bLiteral[ui])
            continue;

        NIASSERT(m_apInOps[ui] != NULL);

        if (m_apInOps[ui]->RetrieveExpectedFinalAccessAndUsage(
            uiAccessFlags, eUsage))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    // From the perspective of the NiOpMergeAndSwizzle, the possible outputs
    // are dependent on the required inputs plust the found optional inputs.
    NIASSERT(m_uiExpectedInputs > 0);

    NiDataStreamElement::Format eExpectedFormat 
        = NiDataStreamElement::F_UNKNOWN;

    // Run through all expected inputs and find the first non-literal format
    for(NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_bLiteral[ui])
        {
            continue;
        }

        NIASSERT(m_apInOps[ui] != NULL);

        if (m_apInOps[ui]->RetrieveExpectedFinalFormat(eExpectedFormat))
        {
            break;
        }
    }

    // Make sure we found a valid format
    if (eExpectedFormat == NiDataStreamElement::F_UNKNOWN)
        return false;

    // Find out the expected type...
    NiDataStreamElement::Type eType = NiDataStreamElement::GetType(
        eExpectedFormat);
    bool bNormalize = NiDataStreamElement::IsNormalized(eExpectedFormat);

    if (eType != NiDataStreamElement::T_UNKNOWN)
    {
        eFormat = NiDataStreamElement::GetPredefinedFormat(eType,
            m_uiExpectedInputs, bNormalize);
    }

    if (eFormat == NiDataStreamElement::F_UNKNOWN)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::CanResolve() const
{
    bool bResult = false; // We require at least one stream.
    for(NiUInt32 ui=0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_bLiteral[ui])
            continue;

        if (m_apInOps[ui] == NULL)
            return false;

        if (m_apInOps[ui]->CanResolve() == false)
            return false;

        bResult = true;
    }
    
    return bResult;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpMergeAndSwizzle::RetrieveFinalFormatAlteringNode()
{
    return this;
}
//---------------------------------------------------------------------------
NiFixedString NiOpMergeAndSwizzle::GetOperationName()
{
    return "MergeAndSwizzle";
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::GetSemantic(NiFixedString& kSemantic, 
    NiUInt8& uiSemanticIndex) const
{
    kSemantic = m_kSemantic;
    uiSemanticIndex = m_uiIndex;
    return true;
}
//---------------------------------------------------------------------------
void NiOpMergeAndSwizzle::ClearOutputRecursive()
{
    for (NiUInt32 ui = 0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_apInOps[ui])
            m_apInOps[ui]->ClearOutputRecursive();
    }

    m_kOutStreamRef.Reset();

    // NiOpMergeAndSwizzle uses its own m_apInOps.
    NIASSERT(!m_spInputOp);
}
//---------------------------------------------------------------------------
void NiOpMergeAndSwizzle::GetOpNodes(const char* pcOperationName, 
    NiTPrimitiveArray<NiOpNode*>& kNodes)
{
    if (GetOperationName() == pcOperationName)
        kNodes.Add(this);

    for (NiUInt32 ui = 0; ui < m_uiExpectedInputs; ui++)
    {
        if (m_apInOps[ui])
            m_apInOps[ui]->GetOpNodes(pcOperationName, kNodes);
    }

    // NiOpMergeAndSwizzle uses its own m_apInOps.
    NIASSERT(!m_spInputOp);
    return;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpMergeAndSwizzle::CopyMembers(NiOpMergeAndSwizzle* pkDest, 
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_uiExpectedInputs = m_uiExpectedInputs;
    pkDest->m_kSemantic = m_kSemantic;
    pkDest->m_uiIndex = m_uiIndex;

    for (NiUInt32 ui = 0; ui < m_uiExpectedInputs; ui++)
    {
        pkDest->m_auiComps[ui] = m_auiComps[ui];
        pkDest->m_bLiteral[ui] = m_bLiteral[ui];
        pkDest->m_adLiterals[ui] = m_adLiterals[ui];

        if (m_apInOps[ui])
            pkDest->m_apInOps[ui] = (NiOpNode*)m_apInOps[ui]->Clone(kCloning);
    }
}
//---------------------------------------------------------------------------
bool NiOpMergeAndSwizzle::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpMergeAndSwizzle* pkOtherNode = 
        NiDynamicCast(NiOpMergeAndSwizzle, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_uiExpectedInputs != m_uiExpectedInputs)
        return false;
    if (pkOtherNode->m_kSemantic != m_kSemantic) 
        return false;
    if (pkOtherNode->m_uiIndex != m_uiIndex) 
        return false;

    if (pkOtherNode->m_uiExpectedInputs != m_uiExpectedInputs)
        return false;

    for (NiUInt32 ui = 0; ui < m_uiExpectedInputs; ui++)
    {
        if (pkOtherNode->m_auiComps[ui] != m_auiComps[ui]) 
            return false;
        if (pkOtherNode->m_bLiteral[ui] != m_bLiteral[ui]) 
            return false;
        if (pkOtherNode->m_adLiterals[ui] != m_adLiterals[ui]) 
            return false;

        if (m_apInOps[ui] && pkOtherNode->m_apInOps[ui] && 
            (pkOtherNode->m_apInOps[ui]->IsEqual(m_apInOps[ui])))
        {
            // do nothing
        }
        else if (m_apInOps[ui] == NULL && pkOtherNode->m_apInOps[ui]== NULL)
        {
            // do nothing
        }
        else
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
