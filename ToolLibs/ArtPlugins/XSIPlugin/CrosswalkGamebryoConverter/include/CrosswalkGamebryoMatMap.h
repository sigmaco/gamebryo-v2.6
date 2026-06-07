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

#ifndef XSI_GAMEBRYO_CONVERTER_MATERIAL_MAP_H
#define XSI_GAMEBRYO_CONVERTER_MATERIAL_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"

#include "CrosswalkGamebryoExtraDataGroup.h"

#include <map>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLBaseMaterial;

class NIMAIN_ENTRY NiMaterialProperty;
class NIMAIN_ENTRY NiSpecularProperty;
class NIMAIN_ENTRY NiMaterial;
template <class T> class NiPointer;
typedef NiPointer<NiMaterialProperty> NiMaterialPropertyPtr;
typedef NiPointer<NiSpecularProperty> NiSpecularPropertyPtr;
typedef NiPointer<NiMaterial> NiMaterialPtr;

namespace epg
{
    class ExtraDataGroup;

    //---------------------------------------------------------------------------
    /// Material map.
    ///
    /// Remembers the mapping between XSI material and corresponding Gamebryo objects.
    //---------------------------------------------------------------------------

    class MaterialMap
    {
    public:
        /// List of Gamebryo material properties.
        typedef std::map<CSLBaseMaterial*,NiMaterialPropertyPtr> MatProperties;

        /// List of Gamebryo specular properties.
        typedef std::map<CSLBaseMaterial*,NiSpecularPropertyPtr> SpecularProperties;

        /// List of Gamebryo materials.
        typedef std::map<CSLBaseMaterial*,NiMaterialPtr> Materials;

        /// List of Gamebryo custom property.
        typedef std::map<CSLBaseMaterial*,ExtraDataGroup*> ExtraDatas;

        /// Create an empty material map.
        MaterialMap();

        /// Empty the material map.
        void Clear();

        /** Add a new mapping between an XSI material and a Gamebryo material property.
            @param CSLBaseMaterial& the XSI material.
            @param NiMaterialProperty& the Gamebryo material property.
        */
        void AddMaterialProperty(CSLBaseMaterial& in_XSIMat, NiMaterialProperty& in_MatProp);

        /** Lookup the Gamebryo material property corresponding to an XSI material in the map.
            @param CSLBaseMaterial& the XSI material we know.
            @return the material property, if any. May return NULL.
        */
        NiMaterialProperty* FindMaterialProperty(CSLBaseMaterial& in_XSIMat) const;

        /** Add a new mapping between an XSI material and a Gamebryo specular property.
            @param CSLBaseMaterial& the XSI material.
            @param NiSpecularProperty& the Gamebryo specular property.
        */
        void AddSpecularProperty(CSLBaseMaterial& in_XSIMat, NiSpecularProperty& in_SpecProp);

        /** Lookup the Gamebryo specular property corresponding to an XSI material in the map.
            @param CSLBaseMaterial& the XSI material we know.
            @return the specular property, if any. May return NULL.
        */
        NiSpecularProperty* FindSpecularProperty(CSLBaseMaterial& in_XSIMat) const;

        /** Add a new mapping between an XSI material and a single-shader material.
            @param CSLBaseMaterial& the XSI material.
            @param NiMaterial& the Gamebryo material.
        */
        void AddMaterial(CSLBaseMaterial& in_XSIMat, NiMaterial& in_GamebryoMat);

        /** Lookup the Gamebryo material corresponding to an XSI material in the map.
            @param CSLBaseMaterial& the XSI material we know.
            @return the material, if any. May return NULL.
        */
        NiMaterial* FindMaterial(CSLBaseMaterial& in_XSIMat) const;

        /** Add a new mapping between an XSI material and extra data describing a shader.
            @param CSLBaseMaterial& the XSI material.
            @param in_pGamebryoExtraDat the extra data.
        */
        void AddExtraDataGroup(CSLBaseMaterial& in_XSIMat, ExtraDataGroup* in_pGamebryoExtraDat);

        /** Lookup the Gamebryo ExtraDataGroup corresponding to an XSI material in the map.
            @param CSLBaseMaterial& the XSI material we know.
            @return the ExtraDataGroup, if any. May return NULL.
        */
        ExtraDataGroup* FindExtraDataGroup(CSLBaseMaterial& in_XSIMat) const;

        /** Retrieve the list of Gamebryo material properties.
            @return the list of properties.
        */
        MatProperties & GetMaterialProperties();

    private:
        // Prevent copies.
        MaterialMap(const MaterialMap &);
        void operator=(const MaterialMap &);

        MatProperties               m_MatProps;
        SpecularProperties          m_SpecularProps;
        Materials                   m_Materials;
        ExtraDatas                  m_ExtraDatas;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_MATERIAL_MAP_H */
