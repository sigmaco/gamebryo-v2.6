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

#include "CrosswalkGamebryoLightingMap.h"
#include "CrosswalkGamebryoEffectMap.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoHelpers.h"

#include "CustomPSet.h"

#include "NiNode.h"
#include "NiLight.h"
#include "NiShadowGenerator.h"
#include "NiShadowManager.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Create an empty shadow map.
    //---------------------------------------------------------------------------
    LightingMap::LightingMap()
    {
    }

    //---------------------------------------------------------------------------
    // Empty the shadow map.
    //---------------------------------------------------------------------------
    void LightingMap::Clear()
    {
        m_LightingGroups.clear();
    }

    //---------------------------------------------------------------------------
    // Add a new light casting shadow to a group.
    //---------------------------------------------------------------------------
    void LightingMap::AddLight(
        const NiFixedString& in_Tag,
        NiLight& in_Light,
        const NiFixedString& in_ShadowTechnique)
    {
        LightingGroup& group = m_LightingGroups[in_Tag];
        group.lights[&in_Light] = in_ShadowTechnique;
    }

    //---------------------------------------------------------------------------
    // Add a node to the lit objects of a light group.
    //---------------------------------------------------------------------------
    void LightingMap::AddLitObject(const NiFixedString& in_Tag, NiNode& in_Node)
    {
        LightingGroup& group = m_LightingGroups[in_Tag];
        group.lit.push_back(&in_Node);
    }

    //---------------------------------------------------------------------------
    // Flag a node as not casting shadows.
    //---------------------------------------------------------------------------
    void LightingMap::DontCastShadows(const NiFixedString& in_Tag, NiNode& in_Node)
    {
        LightingGroup& group = m_LightingGroups[in_Tag];
        group.ignoredCasters.push_back(&in_Node);
    }

    //---------------------------------------------------------------------------
    // Flag a node as not receiving shadows.
    //---------------------------------------------------------------------------
    void LightingMap::DontReceiveShadow(const NiFixedString& in_Tag, NiNode& in_Node)
    {
        LightingGroup& group = m_LightingGroups[in_Tag];
        group.ignoredReceivers.push_back(&in_Node);
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of lighting groups.
    //---------------------------------------------------------------------------
    const LightingMap::LightingGroups& LightingMap::GetLightingGroups() const
    {
       return m_LightingGroups;
    }

    //---------------------------------------------------------------------------
    // Parse a string for a list of tags.
    //---------------------------------------------------------------------------
    void ParseTags(const char* in_Text, std::vector<NiFixedString>& out_Tags)
    {
        out_Tags.resize(0);
        if (!in_Text || !in_Text[0])
            return;

        std::string text(in_Text);
        static const char separators[] = " ,;";
        for (std::string::size_type pos = 0; pos != std::string::npos && pos < text.size(); )
        {
            std::string::size_type sepPos = text.find_first_of(separators, pos);
            if (std::string::npos == sepPos)
                sepPos = text.size();

            std::string tag = text.substr(pos, sepPos - pos);
            if (tag.size() > 0)
                out_Tags.push_back(NiFixedString(tag.c_str()));

            pos = text.find_first_not_of(separators, sepPos);
        }
    }

    //---------------------------------------------------------------------------
    // Load the light group and shadow technique.
    //---------------------------------------------------------------------------
    bool LoadLightingProperty(
        CSLTemplate& in_Tmpl,
        NiFixedString& out_ShadowTechnique,
        std::vector<NiFixedString>& out_Tags)
    {
        out_ShadowTechnique = "";
        out_Tags.resize(0);

        CSLCustomPSet* propSet = FindCustomProperties(
            in_Tmpl, XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME);
        if (!propSet)
            return false;

        out_ShadowTechnique = GetStringParameter(
            *propSet, XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME);

        const char* tags = GetStringParameter(*propSet, XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME);
        ParseTags(tags, out_Tags);

        return true;
    }

    //---------------------------------------------------------------------------
    // Load the list groups, ignored casters and ignored receivers.
    //---------------------------------------------------------------------------
    bool LoadShadowProperty(
        CSLTemplate& in_Tmpl,
        std::vector<NiFixedString>& out_Tags,
        std::vector<NiFixedString>& out_IgnoredCasterTags,
        std::vector<NiFixedString>& out_IgnoredReceiverTags)
    {
        out_Tags.resize(0);
        out_IgnoredCasterTags.resize(0);
        out_IgnoredReceiverTags.resize(0);

        CSLCustomPSet* propSet = FindCustomProperties(
            in_Tmpl, XSI_GAMEBRYO_SHADOW_PROPERTY_NAME);
        if (!propSet)
            return false;

        const char* tags = GetStringParameter(
            *propSet, XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME);
        ParseTags(tags, out_Tags);

        const char* casters = GetStringParameter(
            *propSet, XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME);
        ParseTags(casters, out_IgnoredCasterTags);

        const char* receivers = GetStringParameter(
            *propSet, XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME);
        ParseTags(receivers, out_IgnoredReceiverTags);

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert a lighting group to a shadow generator and register lit objects.
    //---------------------------------------------------------------------------
    bool ConvertLightingGroup(Context& io_Context, const LightingGroup& in_Group)
    {
        EffectMap& effects = io_Context.GetEffectMap();

        typedef LightingGroup::Lights::const_iterator lightIter;
        for (lightIter light = in_Group.lights.begin(); light != in_Group.lights.end(); ++light)
        {
            typedef LightingGroup::Nodes::const_iterator nodeIter;
            for (nodeIter node = in_Group.lit.begin(); node != in_Group.lit.end(); ++node)
                effects.AddAffectedObject(*(light->first), **node);

            const NiFixedString& shadowTechnique = light->second;
            if (shadowTechnique.GetLength() > 0)
            {
                if (XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_NAME != shadowTechnique)
                {
                    NiShadowGenerator* pGenerator = NiNew NiShadowGenerator(light->first);
                    pGenerator->SetShadowTechnique(shadowTechnique);
                    NiShadowManager::AddShadowGenerator(pGenerator);

                    for (nodeIter node = in_Group.ignoredCasters.begin();
                         node != in_Group.ignoredCasters.end();
                         ++node)
                    {
                        pGenerator->AttachUnaffectedCasterNode(*node);
                    }

                    for (nodeIter node = in_Group.ignoredReceivers.begin();
                         node != in_Group.ignoredReceivers.end();
                         ++node)
                    {
                        pGenerator->AttachUnaffectedReceiverNode(*node);
                    }
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
