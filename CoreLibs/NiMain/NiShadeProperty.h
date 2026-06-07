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

#ifndef NISHADEPROPERTY_H
#define NISHADEPROPERTY_H

#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiShadeProperty);

class NIMAIN_ENTRY NiShadeProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiShadeProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiShadeProperty();

    inline void SetSmooth(bool bSmooth);
    inline bool GetSmooth() const;

    inline bool IsEqualFast(const NiShadeProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiShadeProperty* GetDefault();

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 1
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        SMOOTH_MASK = 0x0001
    };

    static NiShadePropertyPtr ms_spDefault;
};

#include "NiShadeProperty.inl"

#endif

