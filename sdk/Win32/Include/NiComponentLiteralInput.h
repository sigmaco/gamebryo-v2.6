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

#ifndef NICOMPONENTLITERALINPUT_H
#define NICOMPONENTLITERALINPUT_H

#include "NiMeshProfileProcessorLibType.h"
#include "NiComponentInput.h"
#include "NiTAdjustValue.h"

/// @cond EMERGENT_INTERNAL
/**
    Represents a literal component input which may may be retrieved
    and casted in the format required. This class is intended to be used with
    the NiMeshProfileOperations::MergeAndSwizzle operation and allows 
    literal component values to be swizzled of merged with other literal 
    component values and / or values from data stream components.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiComponentLiteralInput :
    public NiComponentInput
{
public:
    NiComponentLiteralInput() : m_dLiteral(0) {};
    bool SetInput(double dLiteral);

    virtual bool GetValue(NiUInt32 uiIndex, float& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiFloat16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt32& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt8& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt32& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt8& val);

    virtual bool AssignBasis(StreamBasis& kBasis) const;
    virtual bool IsCompatible(const StreamBasis& kBasis) const;

protected:
    double m_dLiteral;
};

#endif // NICOMPONENTLITERALINPUT_H