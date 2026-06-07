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

#include "CrosswalkGamebryoEffectMap.h"

#include "Template.h"

#include "NiDynamicEffect.h"
#include "NiNode.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Create an empty effect map.
    //---------------------------------------------------------------------------
    EffectMap::EffectMap()
    {
        // Do nothing.
    }

    //---------------------------------------------------------------------------
    // Empty the effect map.
    //---------------------------------------------------------------------------
    void EffectMap::Clear()
    {
        m_DynamicEffects.clear();
        m_AffectedMap.clear();
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI model and a Gamebryo object.
    //---------------------------------------------------------------------------
    void EffectMap::AddEffect(CSLTemplate& in_XSITmpl, NiDynamicEffect& in_GamebryoEffect)
    {
        m_DynamicEffects.insert(DynamicEffects::value_type(&in_XSITmpl, &in_GamebryoEffect));
    }

    //---------------------------------------------------------------------------
    // Add an object being affected by the dynamic effects of a material.
    //---------------------------------------------------------------------------
    bool EffectMap::AddAffectedObject(CSLTemplate& in_XSITmpl, NiNode& in_Node)
    {
        typedef DynamicEffects::iterator iter;
        const iter end = m_DynamicEffects.upper_bound(&in_XSITmpl);
        iter pos = m_DynamicEffects.lower_bound(&in_XSITmpl);
        if (pos != end)
        {
            for (; pos != end; ++pos)
            {
                m_AffectedMap[pos->second].push_back(&in_Node);
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Add an object being affected by a dynamic effect.
    //---------------------------------------------------------------------------
    void EffectMap::AddAffectedObject(NiDynamicEffect& in_Effect, NiNode& in_Node)
    {
        m_AffectedMap[&in_Effect].push_back(&in_Node);
    }

    //---------------------------------------------------------------------------
    // Set the affected list of object to be empty.
    //---------------------------------------------------------------------------
    void EffectMap::SetNoAffectedObject(NiDynamicEffect& in_Effect)
    {
        m_AffectedMap[&in_Effect].resize(0);
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of Gamebryo dynamic effects.
    //---------------------------------------------------------------------------
    const EffectMap::DynamicEffects & EffectMap::GetEffects() const
    {
        return m_DynamicEffects;
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of objects affected by a dynamic effect.
    //---------------------------------------------------------------------------
    const EffectMap::AffectedObjects * EffectMap::FindAffectedObjects(
        NiDynamicEffect& in_GamebryoEffect) const
    {
        typedef AffectedMap::const_iterator iter;
        const iter pos = m_AffectedMap.find(&in_GamebryoEffect);
        if (pos != m_AffectedMap.end())
            return &(pos->second);
        else
            return NULL;
    }
}
