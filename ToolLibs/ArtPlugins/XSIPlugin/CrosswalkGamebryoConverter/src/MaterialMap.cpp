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

#include "CrosswalkGamebryoMatMap.h"

#include "XSIMaterial.h"

#include "NiMaterialProperty.h"
#include "NiSpecularProperty.h"
#include "NiMaterial.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Create an empty material map.
    //---------------------------------------------------------------------------
    MaterialMap::MaterialMap()
    {
        // Do nothing.
    }

    //---------------------------------------------------------------------------
    // Empty the material map.
    //---------------------------------------------------------------------------
    void MaterialMap::Clear()
    {
        m_MatProps.clear();
        m_SpecularProps.clear();
        m_Materials.clear();
        m_ExtraDatas.clear();
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI material and a Gamebryo material property.
    //---------------------------------------------------------------------------
    void MaterialMap::AddMaterialProperty(
        CSLBaseMaterial& in_XSIMat,
        NiMaterialProperty& in_GamebryoMatProp)
    {
        NIASSERT(m_MatProps.count(&in_XSIMat) == 0);
        m_MatProps[&in_XSIMat] = &in_GamebryoMatProp;
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo material property corresponding to an XSI material in the map.
    //---------------------------------------------------------------------------
    NiMaterialProperty* MaterialMap::FindMaterialProperty(CSLBaseMaterial& in_XSIMat) const
    {
        typedef MatProperties::const_iterator iter;
        const iter pos = m_MatProps.find(&in_XSIMat);
        if (pos != m_MatProps.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI material and a Gamebryo specular property.
    //---------------------------------------------------------------------------
    void MaterialMap::AddSpecularProperty(
        CSLBaseMaterial& in_XSIMat,
        NiSpecularProperty& in_GamebryoProp)
    {
        NIASSERT(m_SpecularProps.count(&in_XSIMat) == 0);
        m_SpecularProps[&in_XSIMat] = &in_GamebryoProp;
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo specular property corresponding to an XSI material in the map.
    //---------------------------------------------------------------------------
    NiSpecularProperty* MaterialMap::FindSpecularProperty(CSLBaseMaterial& in_XSIMat) const
    {
        typedef SpecularProperties::const_iterator iter;
        const iter pos = m_SpecularProps.find(&in_XSIMat);
        if (pos != m_SpecularProps.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI material and a single-shader material.
    //---------------------------------------------------------------------------
    void MaterialMap::AddMaterial(CSLBaseMaterial& in_XSIMat, NiMaterial& in_GamebryoMat)
    {
        NIASSERT(m_Materials.count(&in_XSIMat) == 0);
        m_Materials[&in_XSIMat] = &in_GamebryoMat;
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo material corresponding to an XSI material in the map.
    //---------------------------------------------------------------------------
    NiMaterial* MaterialMap::FindMaterial(CSLBaseMaterial& in_XSIMat) const
    {
        typedef Materials::const_iterator iter;
        const iter pos = m_Materials.find(&in_XSIMat);
        if (pos != m_Materials.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI material and extra data describing a shader.
    //---------------------------------------------------------------------------
    void MaterialMap::AddExtraDataGroup(
        CSLBaseMaterial& in_XSIMat,
        ExtraDataGroup* in_pGamebryoExtraDat)
    {
        NIASSERT(m_ExtraDatas.count(&in_XSIMat) == 0);
        m_ExtraDatas[&in_XSIMat] = in_pGamebryoExtraDat;
    }

    //---------------------------------------------------------------------------     
    // Lookup the Gamebryo ExtraDataGroup corresponding to an XSI material in the map.
    //---------------------------------------------------------------------------
    ExtraDataGroup* MaterialMap::FindExtraDataGroup(CSLBaseMaterial& in_XSIMat) const
    {
        typedef ExtraDatas::const_iterator iter;
        const iter pos = m_ExtraDatas.find(&in_XSIMat);
        if (pos != m_ExtraDatas.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of Gamebryo material properties.
    //---------------------------------------------------------------------------
    MaterialMap::MatProperties & MaterialMap::GetMaterialProperties()
    {
        return m_MatProps;
    }
}
