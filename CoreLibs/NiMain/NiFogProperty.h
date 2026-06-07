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

#ifndef NIFOGPROPERTY_H
#define NIFOGPROPERTY_H

#include "NiColor.h"
#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiFogProperty);

class NIMAIN_ENTRY NiFogProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiFogProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    // fog attenuation function
    enum FogFunction
    {
        FOG_Z_LINEAR,
        FOG_RANGE_SQ,
        FOG_VERTEX_ALPHA
    };

    NiFogProperty();

    inline void SetFog(bool bFog);
    inline bool GetFog() const;

    inline void SetFogFunction(FogFunction eFunc);
    FogFunction GetFogFunction() const;

    inline void SetFogColor(const NiColor& kColor);
    inline const NiColor& GetFogColor() const;

    // depth = 0, no fogging
    // depth = 1, fog starts at near plane
    // depth > 1, gives fog starting in front of near plane
    inline void SetDepth(float fDepth);
    inline float GetDepth() const;

    inline bool IsEqualFast(const NiFogProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiFogProperty* GetDefault();

    // *** begin Emergent internal use only ***

    static char* GetViewerString(const char* pcPrefix, FogFunction eFunc);
    static void _SDMInit();
    static void _SDMShutdown();

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
        FOG_MASK        = 0x0001,
        FOG_FUNC_MASK   = 0x0006,
        FOG_FUNC_POS    = 1
    };

    float m_fDepth;
    NiColor m_kColor;

    static NiFogPropertyPtr ms_spDefault;
};

#include "NiFogProperty.inl"

#endif // NIFOGPROPERTY_H
