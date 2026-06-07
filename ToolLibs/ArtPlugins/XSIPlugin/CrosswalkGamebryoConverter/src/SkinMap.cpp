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

#include "CrosswalkGamebryoSkinMap.h"

#include "NiAVObject.h"

namespace epg
{

    //---------------------------------------------------------------------------
    // Create an empty skin map.
    //---------------------------------------------------------------------------
    SkinMap::SkinMap()
    {
    }

    //---------------------------------------------------------------------------
    // Empty the skin map.
    //---------------------------------------------------------------------------
    void SkinMap::Clear()
    {
        m_Bones.clear();
        m_Skins.clear();
    }

    //---------------------------------------------------------------------------
    // Add a new XSI model being skinned.
    //---------------------------------------------------------------------------
    BonesDescriptor& SkinMap::AddBones(CSLModel& in_Model)
    {
        // Note: a model can have many bones, so collisions are OK.
        return m_Bones[&in_Model];
    }

    //---------------------------------------------------------------------------
    // Lookup the bones corresponding to an XSI model being skinned.
    //---------------------------------------------------------------------------
    BonesDescriptor* SkinMap::FindBones(CSLModel& in_Model)
    {
        typedef SkinVertexBones::iterator iter;
        const iter pos = m_Bones.find(&in_Model);
        if (pos != m_Bones.end())
            return &pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Add a new mesh being skinned.
    //---------------------------------------------------------------------------
    SkinDescriptor& SkinMap::AddSkin(NiMesh& in_Mesh)
    {
        // Note: a model can have many bones, so collisions are OK.
        return m_Skins[&in_Mesh];
    }

    //---------------------------------------------------------------------------
    // Lookup the skin corresponding to a mesh being skinned.
    //---------------------------------------------------------------------------
    SkinDescriptor* SkinMap::FindSkin(NiMesh& in_Mesh)
    {
        typedef Skins::iterator iter;
        const iter pos = m_Skins.find(&in_Mesh);
        if (pos != m_Skins.end())
            return &pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of Gamebryo skins.
    //---------------------------------------------------------------------------
    SkinMap::Skins& SkinMap::GetSkins()
    {
        return m_Skins;
    }

    //---------------------------------------------------------------------------
}
