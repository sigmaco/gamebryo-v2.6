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

#ifndef NIOPCOMPONENTPACK_H
#define NIOPCOMPONENTPACK_H

#include "NiOpNode.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::Clamp.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpComponentPack : public NiOpNode
{
    NiDeclareClone(NiOpComponentPack);

public:

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();

    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& eFormat) const;
    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    // Specialized
    void SetParameters(NiDataStreamElement::Format ePackFormat);

    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiDataStreamElement::Format m_ePackFormat;
};
NiSmartPointer(NiOpComponentPack);
#endif // NIOPCOMPONENTPACK_H