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

#ifndef NICOMPONENTSTREAMINPUT_H
#define NICOMPONENTSTREAMINPUT_H

#include <NiDataStreamRef.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiComponentInput.h"

class StreamElementHelperLock;

/// @cond EMERGENT_INTERNAL
/**
    Represents a stream component input which may may be retrieved
    and casted in the format required. This class is intended to be used with
    the NiMeshProfileOperations::MergeAndSwizzle operation and allows 
    stream component values to be swizzled of merged with other data stream 
    component values and / or values from literal components.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiComponentStreamInput :
    public NiComponentInput
{
public:
    NiComponentStreamInput() : m_pkLock(0){};
    bool SetInput(NiDataStreamRef& kStreamRef,
        NiUInt8 uiComp);

    virtual bool GetValue(NiUInt32 uiIndex, float& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiFloat16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt32& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt8& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt32& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt16& val);
    virtual bool GetValue(NiUInt32 uiIndex, NiInt8& val);

    virtual void LockInput();
    virtual void UnlockInput();

    virtual bool AssignBasis(StreamBasis& kBasis) const;
    virtual bool IsCompatible(const StreamBasis& kBasis) const;

protected:
    template<typename T> bool GetTValue(NiUInt32 uiIndex, T& val);

    NiUInt8 m_uiComponentOffset;
    NiDataStreamElement::Type m_eElementType;
    NiDataStreamRef m_kStreamRef;
    StreamElementHelperLock* m_pkLock;
};

#endif // NICOMPONENTSTREAMINPUT_H
