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

#ifndef NIWIREFRAMEPROPERTY_H
#define NIWIREFRAMEPROPERTY_H

#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiWireframeProperty);

class NIMAIN_ENTRY NiWireframeProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiWireframeProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiWireframeProperty();

    inline void SetWireframe(bool bWireframe);
    inline bool GetWireframe() const;

    inline bool IsEqualFast(const NiWireframeProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiWireframeProperty* GetDefault();

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
        WIREFRAME_MASK  = 0x0001
    };

    static NiWireframePropertyPtr ms_spDefault;
};

#include "NiWireframeProperty.inl"

#endif

