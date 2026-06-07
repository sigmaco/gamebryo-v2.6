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

#ifndef NICOMMONSEMANTICS_H
#define NICOMMONSEMANTICS_H

#include "NiMainLibType.h"
#include "NiFixedString.h"

class NIMAIN_ENTRY NiCommonSemantics
{
public:
    // The following enumerations and functions return the 
    // common varying parameter semantics used by shader programs.

    // Always explicitly specify the integral value for the enumeration
    // so the back compatibility is easier when the enumeration changes. 

    // Note: Update the following file when changing the semantics:
    // sdk/Win32/ToolPlugins/Data/MeshProfiles/NiMeshProfileSchema.xsd

    enum Enumeration
    {
        // Invalid
        E__Invalid                  = 0,

        // Vertex Semantics
        E_POSITION                  = 1,
        E_NORMAL                    = 2,
        E_BINORMAL                  = 3,
        E_TANGENT                   = 4,
        E_TEXCOORD                  = 5,
        E_BLENDWEIGHT               = 6,
        E_BLENDINDICES              = 7,
        E_COLOR                     = 8,
        E_PSIZE                     = 9,
        E_TESSFACTOR                = 10,
        E_DEPTH                     = 11,
        E_FOG                       = 12,
        E_POSITIONT                 = 13,
        E_SAMPLE                    = 14,
        E_DATASTREAM                = 15,
        E_INDEX                     = 16,

        // Skinning Semantics
        E_BONEMATRICES              = 17,
        E_BONE_PALETTE              = 18,
        E_UNUSED0                   = 19, // unused
        E_POSITION_BP               = 20,
        E_NORMAL_BP                 = 21,
        E_BINORMAL_BP               = 22,
        E_TANGENT_BP                = 23,

        // Morph Weights Semantic
        E_MORPHWEIGHTS              = 24,

        // Normal sharing semantics, for use in runtime normal calculation
        E_NORMALSHAREINDEX          = 25,
        E_NORMALSHAREGROUP          = 26,

        // Instancing Semantics
        E_TRANSFORMS                = 27,
        E_INSTANCETRANSFORMS        = 28,

        // Display List Semantics
        E_DISPLAYLIST               = 29,

        // Count
        E__EnumerationCount         = 30
    };

    // Vertex semantic accessors
    static const NiFixedString& POSITION();
    static const NiFixedString& NORMAL();
    static const NiFixedString& BINORMAL();
    static const NiFixedString& TANGENT();
    static const NiFixedString& TEXCOORD();
    static const NiFixedString& BLENDWEIGHT();
    static const NiFixedString& BLENDINDICES();
    static const NiFixedString& COLOR();
    static const NiFixedString& PSIZE();
    static const NiFixedString& TESSFACTOR();
    static const NiFixedString& DEPTH();
    static const NiFixedString& FOG();
    static const NiFixedString& POSITIONT();
    static const NiFixedString& SAMPLE();
    static const NiFixedString& DATASTREAM();
    static const NiFixedString& INDEX();

    // Skinning semantic accessors
    static const NiFixedString& BONEMATRICES();
    static const NiFixedString& BONE_PALETTE();
    static const NiFixedString& POSITION_BP();
    static const NiFixedString& NORMAL_BP();
    static const NiFixedString& BINORMAL_BP();
    static const NiFixedString& TANGENT_BP();

    // Morph Weights Semantic accessor
    static const NiFixedString& MORPHWEIGHTS();

    // Normal sharing semantic accessors
    static const NiFixedString& NORMALSHAREINDEX();
    static const NiFixedString& NORMALSHAREGROUP();

    // Instancing Semantic accessors
    static const NiFixedString& TRANSFORMS();
    static const NiFixedString& INSTANCETRANSFORMS();

    // Display List Semantic accessors
    static const NiFixedString& DISPLAYLIST();

    static const NiFixedString& GetString(Enumeration e);

    static Enumeration GetEnumeration(const NiFixedString& kString);

    /*** begin Emergent internal use only ***/
    static void _SDMInit();
    static void _SDMShutdown();
    /*** end Emergent internal use only ***/

protected:
    static NiFixedString ms_kStrings[E__EnumerationCount];
};

#include "NiCommonSemantics.inl"

#endif // NICOMMONSEMANTICS_H
