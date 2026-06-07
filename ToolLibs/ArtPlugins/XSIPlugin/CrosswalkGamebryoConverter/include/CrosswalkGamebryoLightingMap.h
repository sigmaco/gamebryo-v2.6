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

#ifndef XSI_GAMEBRYO_CONVERTER_LIGHTING_MAP_H
#define XSI_GAMEBRYO_CONVERTER_LIGHTING_MAP_H

#include "SLTypes.h"

#include "NiFixedString.h"
#include "NiString.h"

#include <map>
#include <vector>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLTemplate;

class NIMAIN_ENTRY NiNode;
class NIMAIN_ENTRY NiLight;
template <class T> class NiPointer;
typedef NiPointer<NiLight> NiLightPtr;
typedef NiPointer<NiNode> NiNodePtr;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    /// Group of objects casting light and casting or receiving shadows.
    //---------------------------------------------------------------------------
    struct LightingGroup
    {
        /// The container of lights in the group with their shadow generation technique, if any.
        typedef std::map<NiLightPtr, NiFixedString> Lights;

        /// The container of node in the group.
        typedef std::vector<NiNodePtr>  Nodes;

        /// List of light with their shadow generation technique, which may be empty.
        Lights  lights;

        /// List of nodes receiving light. By default, they cast and receive shadows too.
        Nodes lit;

        /// List of lit nodes *not* casting shadows.
        Nodes  ignoredCasters;

        /// List of lit nodes *not* receiving shadows.
        Nodes  ignoredReceivers;
    };

    //---------------------------------------------------------------------------
    // Helpers for the lighting property.
    //---------------------------------------------------------------------------

    /// Convert a lighting group to a shadow generator and register lit objects.
    bool ConvertLightingGroup(Context& io_Context, const LightingGroup& in_Group);

    /** Load the XSI lightning custom property.
        @param in_Tmpl the light to load from.
        @param out_ShadowTechnique the shadow producing technique, if any.
        @param out_Tags the list of tags associated with the object.
    */
    bool LoadLightingProperty(
        CSLTemplate& in_Tmpl,
        NiFixedString& out_ShadowTechnique,
        std::vector<NiFixedString>& out_Tags);

    /** Load the XSI shadow custom property.
        @param in_Tmpl the mesh to load from.
        @param out_Tags the list of tags associated with the object.
        @param out_IgnoredCasterTags the list of ignored caster tags.
        @param out_IgnoredReceiverTags the list of ignored receiver tags.
    */
    bool LoadShadowProperty(
        CSLTemplate& in_Tmpl,
        std::vector<NiFixedString>& out_Tags,
        std::vector<NiFixedString>& out_IgnoredCasterTags,
        std::vector<NiFixedString>& out_IgnoredReceiverTags);

    /** Parse a light tag list.
        @param in_Text the text to parse.
        @param out_Tags the tags found.
    */
    void ParseTags(const char* in_Text, std::vector<NiFixedString>& out_Tags);

    //---------------------------------------------------------------------------
    /// Mapping between light and meshes that cast and receive shadows.
    //---------------------------------------------------------------------------

    class LightingMap
    {
    public:
        /// List of lighting groups indexed by tags.
        typedef std::map<NiString,LightingGroup> LightingGroups;

        /// Create an empty shadow map.
        LightingMap();

        /// Empty the shadow map.
        void Clear();

        /** Add a new light casting shadow to a group.
            @param in_Tag the tag for the group.
            @param in_Light the light to be added.
        */
        void AddLight(const NiFixedString& in_Tag, NiLight& in_Li, const NiFixedString& in_Tech);

        /** Add a mesh to the lit objects of a light group.
            @param in_Tag the tag of the group.
            @param in_Node the node to be lit.
        */
        void AddLitObject(const NiFixedString& in_Tag, NiNode& in_Node);

        /** Flag a mesh as not casting shadows.
            @param in_Tag the tag of the group.
            @param in_Node the node not casting shadows.
        */
        void DontCastShadows(const NiFixedString& in_Tag, NiNode& in_Node);

        /** Flag a mesh as not receiving shadows.
            @param in_Tag the tag of the group.
            @param in_Node the node not receiving shadows.
        */
        void DontReceiveShadow(const NiFixedString& in_Tag, NiNode& in_Node);

        /// Retrieve the list of lighting groups.
        const LightingGroups& GetLightingGroups() const;

    private:
        // Prevent copies.
        LightingMap(const LightingMap &);
        void operator=(const LightingMap &);

        LightingGroups        m_LightingGroups;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_LIGHTING_MAP_H */
