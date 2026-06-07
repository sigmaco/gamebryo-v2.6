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

#ifndef NIOPCASTCONVERT_H
#define NIOPCASTCONVERT_H

#include "NiOpNode.h"
#include "NiMeshProfileOperations.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::CastConvert.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpCastConvert : public NiOpNode
{
    NiDeclareClone(NiOpCastConvert);

public:
    NiOpCastConvert();

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();
    
    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& eFormat) const;

    bool SetParameters( NiDataStreamElement::Type eDstTyp,
        NiMeshProfileOperations::CastConvertType eConvertType);

    NiDataStreamElement::Type GetFormatType() const;
    NiMeshProfileOperations::CastConvertType GetConvertType() const;

    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiDataStreamElement::Type m_eDstType;
    NiMeshProfileOperations::CastConvertType m_eConvertType;
};

#endif // NIOPCASTCONVERT_H