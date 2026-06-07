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

#ifndef NIMESHERRORINTERFACE_H
#define NIMESHERRORINTERFACE_H

#include "NiMeshLibType.h"

/**
    Provides an interface for which to report annotations, errors, and/or
    warnings.
    
    No implementation is provided by NiMesh.
*/
class NIMESH_ENTRY NiMeshErrorInterface
{
public:
    /// @cond EMERGENT_INTERNAL

    // Adds the specified error to the collection of errors.
    virtual void ReportAnnotation(const char* pcFormat, ...) = 0;
    virtual void ReportError(const char* pcFormat, ...) = 0;
    virtual void ReportWarning(const char* pcFormat, ...) = 0;

    virtual ~NiMeshErrorInterface() {}

    /// @endcond
};

#endif // NIMESHERRORINTERFACE_H
