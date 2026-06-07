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


//---------------------------------------------------------------------------
inline void NiPhysXParticleConverter::ReportSequenceConversionError(
    const char* pcSequenceName, const char* pcCtlrType)
{
    NI_UNUSED_ARG(pcSequenceName);
    NI_UNUSED_ARG(pcCtlrType);
    NIASSERT(pcCtlrType);
    NILOG("NiControllerSequence Conversion Error: %s\n\tUnable to properly "
        "convert a reference to a controller of type %s.\n\tThe interpolator "
        "may not get property attached to that controller.\n\tRe-export the "
        "sequence to fix this problem.\n", pcSequenceName, pcCtlrType);
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPhysXParticleConverter::LookupObject(
    NiAVObject* pkOriginalObject,
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiAVObject* pkNewObject;
    if (kConversionMap.GetAt(pkOriginalObject, pkNewObject))
    {
        return pkNewObject;
    }

    return pkOriginalObject;
}
//---------------------------------------------------------------------------
