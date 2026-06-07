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

#ifndef NIMESHPROFILEGROUP_H
#define NIMESHPROFILEGROUP_H

// Set the number of renderer to be one more to support the generic renderer
#define MESHPROFILE_PARSER_NUM_RENDERERS NiSystemDesc::RENDERER_NUM + 1

#include <NiTArray.h>

#include "NiMeshProfileXMLParserLibType.h"
#include "NiMeshProfile.h"

/// @cond EMERGENT_INTERNAL
/**
    This is the mesh profile group class.  It is used to store mesh profiles
    by there renderer type.  This is used to gain access to the individual 
    mesh profiles either by name or by index.
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfileGroup : public NiMemObject
{
public:
    NiMeshProfileGroup();
    virtual ~NiMeshProfileGroup();

    // Accessors
    inline NiUInt32 GetProfileCount(NiSystemDesc::RendererID eRenderer) const;
    inline NiUInt32 GetProfileCount() const;
    inline const NiMeshProfile* GetProfile(NiSystemDesc::RendererID eRenderer,
        NiUInt32 uiIndex) const;
    const NiMeshProfile* GetProfile(const NiFixedString& pProfileName, 
        NiSystemDesc::RendererID eRenderer);
    bool Validate();
    void PrintDescription();

    // Modifiers
    NiMeshProfile* NewProfile(NiSystemDesc::RendererID eRenderer);

protected:

    /// The array of mesh profiles
    NiTObjectArray<NiMeshProfilePtr> 
        m_akProfiles[MESHPROFILE_PARSER_NUM_RENDERERS];
    
};

#include "NiMeshProfileGroup.inl"

#endif  // #ifndef NIMESHPROFILEGROUP_H
