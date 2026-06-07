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

#ifndef NIOPMERGEANDSWIZZLE_H
#define NIOPMERGEANDSWIZZLE_H

#include "NiOpNode.h"
#include "NiComponentInput.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::MergeAndSwizzle.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpMergeAndSwizzle : public NiOpNode
{
    NiDeclareClone(NiOpMergeAndSwizzle);

public:
    NiOpMergeAndSwizzle();
    virtual ~NiOpMergeAndSwizzle();

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual void ClearOutputRecursive();

    virtual bool SetInput(NiOpNode* pkOp);

    virtual NiFixedString GetOperationName();

    virtual bool GetSemantic(NiFixedString& kSemantic, 
        NiUInt8& uiSemanticIndex) const;

    virtual void GetOpNodes(const char* pcOperationName, 
        NiTPrimitiveArray<NiOpNode*>& kNodes);

    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& kFormat) const;

    virtual bool RetrieveExpectedFinalAccessAndUsage(NiUInt8& uiAccessFlags,
        NiDataStream::Usage& eUsage) const;

    virtual bool CanResolve() const;

    bool SetExpectedInputs(NiUInt8 uiExpectedInputs);

    bool SetInput(NiUInt8 uiInput, NiOpNode* pkInOp, NiUInt8 uiComp,
        bool bRequired = true);
    bool SetInput(NiUInt8 uiInput, double dValue);
    bool SetParameters(const NiFixedString& kSemantic, NiUInt8 uiIndex);

    bool UseLiteral(const NiUInt32 ui) const;

    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiUInt8 m_uiExpectedInputs;
    NiUInt8 m_auiComps[4];
    NiUInt8 m_bLiteral[4];
    NiOpNodePtr m_apInOps[4];
    double m_adLiterals[4];
    NiFixedString m_kSemantic;
    NiUInt8 m_uiIndex;
};
NiSmartPointer(NiOpMergeAndSwizzle);

#endif // NIOPMERGEANDSWIZZLE_H