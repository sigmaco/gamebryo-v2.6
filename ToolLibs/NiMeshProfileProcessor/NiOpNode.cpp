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

#include "NiOpNode.h"

NiImplementCreateClone(NiOpNode);

//---------------------------------------------------------------------------
NiOpNode::NiOpNode()
{
    m_spInputOp = NULL;
}
//---------------------------------------------------------------------------
NiOpNode::~NiOpNode()
{
}
//---------------------------------------------------------------------------
NiDataStreamRef& NiOpNode::GetOutput()
{
    return m_kOutStreamRef;
}
//---------------------------------------------------------------------------
bool NiOpNode::GetOutputs(NiTObjectSet<NiDataStreamRef>&
    aStreamRefs)
{
    // default behavior is to return 1 output
    aStreamRefs.Add(m_kOutStreamRef);
    return true;
}
//---------------------------------------------------------------------------
bool NiOpNode::SetInput(NiOpNode* pkOp)
{
    m_spInputOp = pkOp;
    return true;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpNode::GetInput() const
{
    return m_spInputOp;
}
//---------------------------------------------------------------------------
NiOpNode* NiOpNode::RetrieveFinalFormatAlteringNode()
{
    return m_spInputOp->RetrieveFinalFormatAlteringNode();
}
//---------------------------------------------------------------------------
void NiOpNode::ClearOutput()
{
    // clears without deletion.
    m_kOutStreamRef.Reset();
}
//---------------------------------------------------------------------------
void NiOpNode::ClearOutputRecursive()
{
    // Call all inputs
    if (m_spInputOp)
        m_spInputOp->ClearOutputRecursive();

    // delete and clear.
    m_kOutStreamRef.Reset();
}
//---------------------------------------------------------------------------
NiFixedString NiOpNode::GetOperationName()
{
    return "Base";
}
//---------------------------------------------------------------------------
bool NiOpNode::GetSemantic(NiFixedString& kSemantic, 
    NiUInt8& uiSemanticIndex) const
{
    if (!m_spInputOp)
        return false;

    return m_spInputOp->GetSemantic(kSemantic, uiSemanticIndex);
}
//---------------------------------------------------------------------------
bool NiOpNode::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    if (!m_spInputOp)
        return false;

    // Default behavior makes no modifications.
    return m_spInputOp->RetrieveExpectedFinalFormat(eFormat);
}
//---------------------------------------------------------------------------
bool NiOpNode::RetrieveExpectedFinalAccessAndUsage(
    NiUInt8& uiAccessFlags, NiDataStream::Usage& eUsage) const
{
    if (!m_spInputOp)
        return false;

    return m_spInputOp->RetrieveExpectedFinalAccessAndUsage(uiAccessFlags,
        eUsage);
}
//---------------------------------------------------------------------------
void NiOpNode::GetOpNodes(const char* pcOperationName, 
    NiTPrimitiveArray<NiOpNode*>& kNodes)
{
    if (GetOperationName() == pcOperationName)
        kNodes.Add(this);

    if (!m_spInputOp)
        return;

    m_spInputOp->GetOpNodes(pcOperationName, kNodes);
}
//---------------------------------------------------------------------------
bool NiOpNode::Resolve(NiMeshErrorInterface*)
{
    return false;
}

//---------------------------------------------------------------------------
bool NiOpNode::CanResolve() const
{
    if (!m_spInputOp)
        return false;

    return m_spInputOp->CanResolve();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpNode::CopyMembers(NiOpNode* pkDest, NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    if (m_spInputOp)
        pkDest->m_spInputOp = (NiOpNode*)m_spInputOp->Clone(kCloning);

    pkDest->m_kOutStreamRef = m_kOutStreamRef;
}
//---------------------------------------------------------------------------
bool NiOpNode::IsEqual(NiObject* pkOther)
{
    if (!NiObject::IsEqual(pkOther))
        return false;

    NiOpNode* pkOtherNode = NiDynamicCast(NiOpNode, pkOther);

    if (!pkOtherNode)
        return false;
    
    if (m_spInputOp == NULL && pkOtherNode->m_spInputOp != NULL)
        return false;
    if (m_spInputOp != NULL && pkOtherNode->m_spInputOp == NULL)
        return false;

    if (m_spInputOp == NULL && pkOtherNode->m_spInputOp == NULL)
    {
        // Do nothing
    }
    else if (!m_spInputOp->IsEqual(pkOtherNode->m_spInputOp))
    {
        return false;
    }

    if (!pkOtherNode->m_kOutStreamRef.IsEqual(&m_kOutStreamRef))
        return false;

    return true;
}
//---------------------------------------------------------------------------
