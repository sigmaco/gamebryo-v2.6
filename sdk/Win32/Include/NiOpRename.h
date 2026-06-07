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

#ifndef NIOPRENAME_H
#define NIOPRENAME_H

#include "NiOpNode.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::Rename.

    @note The rename operation, which only needs to change the semantic and
        index on the datastream ref, is the only operation that continues
        to share the underlying datastream from its input.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpRename : public NiOpNode
{
    NiDeclareClone(NiOpRename);

public:

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();

    bool SetParameters(const NiFixedString& kSemantic, NiUInt8 uiIndex);

    virtual bool GetSemantic(NiFixedString& kSemantic, 
        NiUInt8& uiSemanticIndex) const;

    virtual bool IsEqual(NiObject* pkOther);
protected:
    NiFixedString m_kSemantic;
    NiUInt8 m_uiIndex;
};
NiSmartPointer(NiOpRename);

#endif // NIOPRENAME_H