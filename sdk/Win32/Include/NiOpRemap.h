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

#ifndef NIOPREMAP_H
#define NIOPREMAP_H

#include "NiOpNode.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::Remap.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpRemap : public NiOpNode
{
    NiDeclareClone(NiOpRemap);

public:

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();

    // Specialized
    void SetParameters(NiInt32 iSelectedComponent, 
        double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd);

    virtual bool IsEqual(NiObject* pkOther);
protected:
    NiInt32 m_iSelectedComponent; // Selected Component
    double m_dSrcBegin;
    double m_dSrcEnd;
    double m_dDstBegin;
    double m_dDstEnd;
};
NiSmartPointer(NiOpRemap);

#endif // NIOPREMAP