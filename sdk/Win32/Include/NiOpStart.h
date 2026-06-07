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

#ifndef NIOPSTART_H
#define NIOPSTART_H

#include "NiOpNode.h"
#include "NiComponentInput.h"

/// @cond EMERGENT_INTERNAL
/**
    Special Operation node that specifies a start stream for which will
    flow toward downstream operations. A Semantic and index is stored in
    order to link up a particular stream from a given mesh.

    Also supports auto-generation, however, in order for that to occur,
    a valid StreamBasis must be set for which to use as a template.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpStart : public NiOpNode
{
    NiDeclareClone(NiOpStart);

public:
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();
    virtual bool GetSemantic(NiFixedString& kSemantic, 
        NiUInt8& uiSemanticIndex) const;
    virtual void GetOpStartNodes(NiTPrimitiveArray<NiOpNode*>& kNodes);

    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& kFormat) const;

    virtual bool RetrieveExpectedFinalAccessAndUsage(NiUInt8& uiAccessFlags,
        NiDataStream::Usage& eUsage) const;

    virtual bool CanResolve() const;

    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    bool IsEqual(const NiFixedString& kSemantic, const NiUInt8 uiIndex) const;

    const NiFixedString& GetSemantic() const;

    NiUInt8 GetSemanticIndex() const;

    bool SetParameters(const NiFixedString& kSemantic, const NiUInt8 uiIndex);

    bool SetOutput(NiDataStreamRef& kStreamRef);

    virtual bool IsEqual(NiObject* pkOther);
    
protected:

    NiFixedString m_kSemantic;
    NiUInt8 m_uiIndex;
};

NiSmartPointer(NiOpStart);

#endif // NIOPSTART_H