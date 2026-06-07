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

#ifndef NIOPNODE_H
#define NIOPNODE_H

#include "NiMeshProfileProcessorLibType.h"
#include <NiMeshErrorInterface.h>
#include <NiTArray.h>
#include <NiTSet.h>
#include <NiObject.h>
#include <NiDataStreamRef.h>

NiSmartPointer(NiOpNode);

/// @cond EMERGENT_INTERNAL
/**
    Base class for operation nodes. Operation nodes are used wrap mesh
    profile operations and more importantly, to allow those operations
    to be strung together in a series of defined operations via input
    and output connections. As operations specify a data flow and do not
    modify their inputs, it is safe to cascade results off into various
    inputs of different operation nodes.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpNode : public NiObject
{
    NiDeclareClone(NiOpNode);

public:
    NiOpNode();
    virtual ~NiOpNode();

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);

    // base default behavior is to support only 1 input.
    virtual bool SetInput(NiOpNode* pkOp);
    virtual NiOpNode* GetInput() const;

    // Following methods to be used only by NiOpNode derived classes only
    virtual bool GetOutputs(NiTObjectSet<NiDataStreamRef>& aStreamRefs);
    virtual NiDataStreamRef& GetOutput();
    virtual void ClearOutput();
    virtual void ClearOutputRecursive();

    virtual NiFixedString GetOperationName();

    virtual bool GetSemantic(NiFixedString& kSemantic, NiUInt8&
        uiSemanticIndex) const;

    virtual void GetOpNodes(const char* pcOperationName, 
        NiTPrimitiveArray<NiOpNode*>& kNodes);

    virtual bool CanResolve() const;
    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& kFormat) const;

    virtual bool RetrieveExpectedFinalAccessAndUsage(NiUInt8& uiAccessFlags,
        NiDataStream::Usage& eUsage) const;

    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    virtual bool IsEqual(NiObject* pkOther);
protected:

    NiOpNodePtr m_spInputOp; // Pointer to input node
    NiDataStreamRef m_kOutStreamRef; // Streamref
};

#endif // NIOPNODE