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

#ifndef NIOPCLAMP_H
#define NIOPCLAMP_H

#include "NiOpNode.h"

/// @cond EMERGENT_INTERNAL
/**
    Operation node to wrap NiMeshProfileOperation::Clamp.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpClamp : public NiOpNode
{
    NiDeclareClone(NiOpClamp);

public:

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();


    // Specialized
    void SetParameters(NiInt32 iSelectedComponent, double dMin, double dMax);

    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiInt32 m_iSelectedComponent; // Selected Component
    double m_dMin;
    double m_dMax;
};
NiSmartPointer(NiOpClamp);
#endif // NIOPCLAMP