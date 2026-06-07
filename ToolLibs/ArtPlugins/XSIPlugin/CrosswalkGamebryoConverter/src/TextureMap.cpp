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

#include "CrosswalkGamebryoTexMap.h"

#include "XSIMaterial.h"

#include "NiTexturingProperty.h"
#include "NiSourceTexture.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Texture Info.
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // Creates an empty texture info.
    //---------------------------------------------------------------------------
    TextureInfo::TextureInfo()
        : m_Property(NiNew NiTexturingProperty)
        , m_NextStreamIndex(0)
    {
    }

    //---------------------------------------------------------------------------
    // Retrieve the texture property.
    //---------------------------------------------------------------------------
    NiTexturingProperty& TextureInfo::GetProperty() const
    {
        return *m_Property;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI texture-space name and a Gamebryo stream index.
    //---------------------------------------------------------------------------
    int TextureInfo::AddStreamIndex(const NiFixedString& in_Name, const RepeatUV& in_RepeatUV)
    {
        typedef TextureRepeats::iterator rep_iter;
        const rep_iter repPos = m_TextureRepeats.find(in_Name);
        if (repPos != m_TextureRepeats.end())
        {
            typedef TextureCoordinates::iterator coord_iter;
            const coord_iter coordPos = repPos->second.find(in_RepeatUV);
            if (coordPos != repPos->second.end())
            {
                return coordPos->second;
            }
        }

        // Note: in XSI, the special name "*" seems to mean to match
        //       the first UV set in the shape...
        if (in_Name == "*")
        {
            m_TextureRepeats[in_Name][in_RepeatUV] = 0;
            const int index = 0;
            return index;
        }
        else
        {
            m_TextureRepeats[in_Name][in_RepeatUV] = m_NextStreamIndex;
            const int index = m_NextStreamIndex++;
            return index;
        }
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo stream index corresponding to an XSI texture-space name.
    //---------------------------------------------------------------------------
    const TextureInfo::TextureCoordinates&
    TextureInfo::FindStreamIndexes(const NiFixedString& in_Name) const
    {
        static const TextureCoordinates empty;

        typedef TextureRepeats::const_iterator iter;
        const iter pos = m_TextureRepeats.find(in_Name);
        if (pos != m_TextureRepeats.end())
            return pos->second;
        else
            return empty;
    }

    //---------------------------------------------------------------------------
    // Texturing map.
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // Create an empty texture map.
    //---------------------------------------------------------------------------
    TextureMap::TextureMap()
    {
        // Do nothing.
    }

    //---------------------------------------------------------------------------
    // Empty the texture map.
    //---------------------------------------------------------------------------
    void TextureMap::Clear()
    {
        m_TextureInfos.clear();
        m_TextureFilenames.clear();
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI material and a Gamebryo texture info.
    //---------------------------------------------------------------------------
    TextureInfo& TextureMap::AddTextureInfo(CSLBaseMaterial& in_XSIMat)
    {
        TextureInfo* info = FindTextureInfo(in_XSIMat);
        if (info)
            return *info;
        else
            return m_TextureInfos[&in_XSIMat];
    }

    //---------------------------------------------------------------------------
    // Lookup the texture info corresponding to an XSI material.
    //---------------------------------------------------------------------------
    TextureInfo* TextureMap::FindTextureInfo(CSLBaseMaterial& in_XSIMat)
    {
        typedef TextureInfos::iterator iter;
        const iter pos = m_TextureInfos.find(&in_XSIMat);
        if (pos != m_TextureInfos.end())
            return &(pos->second);
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of Gamebryo texture properties.
    //---------------------------------------------------------------------------
    TextureMap::TextureInfos & TextureMap::GetTextureInfos()
    {
        return m_TextureInfos;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI image name and a texture filename.
    //---------------------------------------------------------------------------
    void TextureMap::AddSourceTexture(const NiFixedString& in_Name, NiFixedString& in_Filename)
    {
        NIASSERT(m_TextureFilenames.count(in_Name) == 0);
        m_TextureFilenames[in_Name] = in_Filename;
    }

    //---------------------------------------------------------------------------
    // Lookup the texture filename corresponding to an XSI image name.
    //---------------------------------------------------------------------------
    const NiFixedString* TextureMap::FindTextureFilename(const NiFixedString& in_Name) const
    {
        typedef TextureFilenames::const_iterator iter;
        const iter pos = m_TextureFilenames.find(in_Name);
        if (pos != m_TextureFilenames.end())
            return &(pos->second);
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of texture filenames.
    //---------------------------------------------------------------------------
    TextureMap::TextureFilenames & TextureMap::GetTextureFilenames()
    {
        return m_TextureFilenames;
    }

    //---------------------------------------------------------------------------
}
