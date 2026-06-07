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

#ifndef NIRENDEROBJECTMATERIALOPTION_H
#define NIRENDEROBJECTMATERIALOPTION_H

#include "NiFixedString.h"

/** 
    NiRenderObjectMaterialOption manages the strings that are used by 
    Gamebryo to query meshes and mesh modifiers for material options. 

    Applications can define their own strings in other files - 
    this class simply provides a single source for those predefined 
    by Gamebryo.
*/

class NIMAIN_ENTRY NiRenderObjectMaterialOption
{
public:
    /// Enumerates the predefined NiRenderObject material options.
    enum Enumeration
    {
        /// Unrecognized material option
        E__Invalid,

        /// Transform vertices with Gamebryo skinning
        E_TRANSFORM_SKINNED,
        /// Transform vertices with Gamebryo instancing
        E_TRANSFORM_INSTANCED,
        /// Transform vertices with Gamebryo morphing
        E_MORPHING,

        /// Number of predefined material options
        E__EnumerationCount
    };
    /// Transform vertices with Gamebryo skinning
    static const NiFixedString& TRANSFORM_SKINNED();

    /// Transform vertices with Gamebryo instancing
    static const NiFixedString& TRANSFORM_INSTANCED();

    /// Transform vertices with Gamebryo morphing
    static const NiFixedString& MORPHING();

    /// Converts a NiRenderObjectMaterialOption::Enumeration to a 
    /// predefined NiFixedString.
    static const NiFixedString& GetString(Enumeration e);

    /// Converts a predefined NiRenderObjectMaterialOption NiFixedString to 
    /// a NiRenderObjectMaterialOption::Enumeration.
    static Enumeration GetEnumeration(const NiFixedString& kString);

    /// @cond EMERGENT_INTERNAL

    static void _SDMInit();
    static void _SDMShutdown();

    /// @endcond
protected:
    static NiFixedString ms_kStrings[E__EnumerationCount];
};


#include "NiRenderObjectMaterialOption.inl"


#endif // #ifndef NIRENDEROBJECTMATERIALOPTION_H
