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

#include <NiDataStreamElement.h>
#include "NiMeshProfileOperations.h"

//---------------------------------------------------------------------------
bool NiMeshProfileOperations::IsValidStreamRef(
    const NiDataStreamRef& kStreamRef)
{
    if (!kStreamRef.IsValid())
        return false;

    // There must be exactly one element (and one element reference)
    if (kStreamRef.GetElementDescCount() != 1)
        return false;

    // Ensure that the element is one of the known types
    NiDataStreamElement kElement = kStreamRef.GetElementDescAt(0);
    if (kElement.GetType() == NiDataStreamElement::T_UNKNOWN)
        return false;

    return true;
}
//---------------------------------------------------------------------------


