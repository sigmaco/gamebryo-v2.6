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

#ifndef NIOPREINTERPRETFORMAT_H
#define NIOPREINTERPRETFORMAT_H

#include "NiOpNode.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::ReinterpretFormat.

    @note ReinterpretFormat may be used to change the format of a datastream
        without changing the data itself. Possible formats also include the
        less standard formats such F_NORMUINT8_4_BGRA. In a chain of
        operations, ReinterpretFormat should be used as one of the final
        operations since some operations may not be equipped to deal
        with special types.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpReinterpretFormat : public NiOpNode
{
    NiDeclareClone(NiOpReinterpretFormat);

public:

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();

    virtual bool RetrieveExpectedFinalFormat(
        NiDataStreamElement::Format& kFormat) const;

    void SetParameters(NiDataStreamElement::Format eDstFormat);

    virtual NiOpNode* RetrieveFinalFormatAlteringNode();

    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiDataStreamElement::Format m_eDstFormat;
};
NiSmartPointer(NiOpReinterpretFormat);

#endif // NIREINTERPRETFORMAT_H