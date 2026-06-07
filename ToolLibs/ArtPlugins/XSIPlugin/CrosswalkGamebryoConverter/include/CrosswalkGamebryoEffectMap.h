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

#ifndef XSI_GAMEBRYO_CONVERTER_EFFECT_MAP_H
#define XSI_GAMEBRYO_CONVERTER_EFFECT_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiString.h"

#include <map>
#include <vector>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLTemplate;

class NIMAIN_ENTRY NiDynamicEffect;
class NIMAIN_ENTRY NiNode;
template <class T> class NiPointer;
typedef NiPointer<NiDynamicEffect> NiDynamicEffectPtr;
typedef NiPointer<NiNode> NiNodePtr;

namespace epg
{
    //---------------------------------------------------------------------------
    /// Effect map.
    ///
    /// Remembers the mapping between XSI template and Gamebryo dynamic effect.
    //---------------------------------------------------------------------------

    class EffectMap
    {
    public:
        /// List of Gamebryo dynamic effects.
        typedef std::multimap<CSLTemplate*,NiDynamicEffectPtr> DynamicEffects;

        /// List of object names being affected by the dynamic effect.
        typedef std::vector<NiNodePtr> AffectedObjects;

        /// Create an empty effect map.
        EffectMap();

        /// Empty the effect map.
        void Clear();

        /** Add a new mapping between an XSI model and a Gamebryo object.
            @param CSLModel& the XSI model.
            @param NiDynamicEffect& the Gamebryo dynamic effect.
        */
        void AddEffect(CSLTemplate& in_XSITmpl, NiDynamicEffect& in_GamebryoEffect);

        /** Add an object being affected by the dynamic effects of a material, if any.
            @param in_XSITmpl the material to find.
            @param in_Node the node the could be affected.
            @return true if one or more dynamic effects really existed.
        */
        bool AddAffectedObject(CSLTemplate& in_XSITmpl, NiNode& in_Node);

        /** Add an object being affected by a dynamic effect.
            @param in_Effect the effect to add the object to.
            @param in_Node the object being affected.
        */
        void AddAffectedObject(NiDynamicEffect& in_Effect, NiNode& in_Node);

        /** Set the affected list of object to be empty.
            @param in_Effect the effect to set an empty affected list on.
        */
        void SetNoAffectedObject(NiDynamicEffect& in_Effect);

        /** Retrieve the list of Gamebryo dynamic effects.
            @return the list of dynamic effects.
        */
        const DynamicEffects& GetEffects() const;

        /// Retrieve the list of objects affected by a dynamic effect.
        const AffectedObjects * FindAffectedObjects(NiDynamicEffect& in_GamebryoEffect) const;

    private:
        // Prevent copies.
        EffectMap(const EffectMap &);
        void operator=(const EffectMap &);

        typedef std::map<NiDynamicEffectPtr,AffectedObjects> AffectedMap;

        DynamicEffects              m_DynamicEffects;
        AffectedMap                 m_AffectedMap;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_EFFECT_MAP_H */
