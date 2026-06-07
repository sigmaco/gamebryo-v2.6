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

#ifndef NIVERTEXCOLORPROPERTY_H
#define NIVERTEXCOLORPROPERTY_H

#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiVertexColorProperty);

class NIMAIN_ENTRY NiVertexColorProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiVertexColorProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiVertexColorProperty();

    enum SourceVertexMode
    {
        SOURCE_IGNORE,
        SOURCE_EMISSIVE,
        SOURCE_AMB_DIFF,
        SOURCE_NUM_MODES
    };

    enum LightingMode
    {
        LIGHTING_E, // Emissive only
        LIGHTING_E_A_D, // Emissive, ambient and diffuse
        LIGHTING_NUM_MODES
    };

    inline void SetSourceMode(SourceVertexMode eSource);
    SourceVertexMode GetSourceMode() const;

    inline void SetLightingMode(LightingMode eLighting);
    LightingMode GetLightingMode() const;

    inline bool IsEqualFast(const NiVertexColorProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiVertexColorProperty* GetDefault();
        
    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // streaming support
    static char* GetViewerString(const char* pcPrefix, 
        SourceVertexMode eMode);
    static char* GetViewerString(const char* pcPrefix, LightingMode eMode);

    // *** end Emergent internal use only ***

protected:
    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 3
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        COLORMODE_MASK          = 0x0007,
        COLORMODE_POS           = 0,
        LIGHTINGMODE_MASK       = 0x0008,
        LIGHTINGMODE_POS        = 3,
        SOURCEVERTEXMODE_MASK   = 0x0030,
        SOURCEVERTEXMODE_POS    = 4
    };

    static NiVertexColorPropertyPtr ms_spDefault;
};

#include "NiVertexColorProperty.inl"

#endif

