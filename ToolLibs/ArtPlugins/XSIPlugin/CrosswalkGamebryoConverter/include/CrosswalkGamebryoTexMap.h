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

#ifndef XSI_GAMEBRYO_CONVERTER_TEXTURE_MAP_H
#define XSI_GAMEBRYO_CONVERTER_TEXTURE_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiFixedString.h"
#include "NiTexturingProperty.h"

#include <map>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLBaseMaterial;

class NIMAIN_ENTRY NiTexturingProperty;
class NIMAIN_ENTRY NiSourceTexture;
typedef NiPointer<NiSourceTexture> NiSourceTexturePtr;

namespace epg
{
    //---------------------------------------------------------------------------
    /// Information about the Gamebryo texture.
    //---------------------------------------------------------------------------

    class TextureInfo
    {
    public:
        /// UV repeat counts used to scale UV coordinates.
        typedef std::pair<float,float> RepeatUV;

        /** List of Gamebryo texture coordinate stream indexes,
            indexed by the repeat counts in UV spaces.
        */
        typedef std::map<RepeatUV,int> TextureCoordinates;

        /** List of repeat factors for texture coordinate,
            indexed by the XSI texture-space name.
        */
        typedef std::map<NiFixedString,TextureCoordinates> TextureRepeats;

        /// Creates an empty texture info.
        TextureInfo();

        /// Retrieve the texture property.
        NiTexturingProperty& GetProperty() const;

        /** Add a new mapping between an XSI texture-space name plus a repeat factor
            and a Gamebryo stream index.
            @param in_Name the XSI texture-space name.
            @param in_RepeatUV the repeat counts in UV space.
            @return the stream index corresponding to the name and repeat counts.
        */
        int AddStreamIndex(const NiFixedString& in_Name, const RepeatUV& in_RepeatUV);

        /** Looup the Gamebryo repeat factor corresponding to an XSI
            @param in_Name the XSI texture-space name.
            @return the list of stream indexes, indexed by repeat counts.
        */
        const TextureCoordinates& FindStreamIndexes(const NiFixedString& in_Name) const;

    private:
        NiTexturingPropertyPtr      m_Property;
        int                         m_NextStreamIndex;
        TextureRepeats              m_TextureRepeats;
    };

    //---------------------------------------------------------------------------
    /// Texture map.
    ///
    /// Remembers the mapping between XSI texture and corresponding Gamebryo objects.
    //---------------------------------------------------------------------------

    class TextureMap
    {
    public:
        /// List of Gamebryo texturing properties, indexed by the XSI material.
        typedef std::map<CSLBaseMaterial*,TextureInfo> TextureInfos;

        /// List of texture file name, indexed by the XSI image name.
        typedef std::map<NiFixedString,NiFixedString> TextureFilenames;

        /// Create an empty texture map.
        TextureMap();

        /// Empty the texture map.
        void Clear();

        /** Add a new mapping between an XSI material and a Gamebryo texture info.
            @param CSLBaseMaterial& the XSI material.
            @return the new or existing texture info corresponding to the XSI material.
        */
        TextureInfo& AddTextureInfo(CSLBaseMaterial& in_XSIMat);

        /** Lookup the texture info corresponding to an XSI material.
            @param CSLBaseMaterial& the XSI material we know.
            @return the texture info, if any. May return NULL.
        */
        TextureInfo* FindTextureInfo(CSLBaseMaterial& in_XSIMat);

        /** Retrieve the list of Gamebryo texture infos.
            @return the list of properties.
        */
        TextureInfos& GetTextureInfos();

        /** Add a new mapping between an XSI image name and a file name.
            @param NiFixedString& the XSI image name.
            @param in_FileName the file name.
        */
        void AddSourceTexture(const NiFixedString& in_Name, NiFixedString& in_FileName);

        /** Lookup the texture filename corresponding to an XSI image name.
            @param NiFixedString& the XSI image name we know.
            @return the texture filename, if any. May return NULL.
        */
        const NiFixedString* FindTextureFilename(const NiFixedString& in_Name) const;

        /** Retrieve the list of Gamebryo source textures.
            @return the list of source textures.
        */
        TextureFilenames& GetTextureFilenames();

    private:
        // Prevent copies.
        TextureMap(const TextureMap &);
        void operator=(const TextureMap &);

        TextureInfos                m_TextureInfos;
        TextureFilenames            m_TextureFilenames;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_TEXTURE_MAP_H */
