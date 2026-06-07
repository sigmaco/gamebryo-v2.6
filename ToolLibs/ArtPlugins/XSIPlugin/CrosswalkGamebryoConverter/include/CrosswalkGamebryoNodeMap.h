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

#ifndef XSI_GAMEBRYO_CONVERTER_NODE_MAP_H
#define XSI_GAMEBRYO_CONVERTER_NODE_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiFixedString.h"

#include <map>
#include <list>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;

class NIMAIN_ENTRY NiAVObject;
class NIMAIN_ENTRY NiNode;
class NIMAIN_ENTRY NiLODNode;
template <class T> class NiPointer;
typedef NiPointer<NiAVObject> NiAVObjectPtr;
typedef NiPointer<NiLODNode> NiLODNodePtr;

namespace epg
{
    //---------------------------------------------------------------------------
    /// Node map. Remembers the mapping between XSI and Gamebryo scene objects.
    //---------------------------------------------------------------------------

    class NodeMap
    {
    public:
        /// List of root Gamebryo objects.
        typedef std::list<NiAVObjectPtr> RootObjects;

        /// Create an empty node map.
        NodeMap();

        /// Empty the node map.
        void Clear();

        /** Reset all objects to be under this root.
            Required because the Gamebryo conversion plugin pipeline
            only support NiNode as root, so we need to add a top
            node to support other free obejcts like lights and effects.
        */
        void Transplant(NiNode& in_SceneRoot);

        /** Add a new mapping between an XSI model and its Gamebryo object equivalent.
            @param in_XSIModel the XSI model that was converted.
            @param in_GamebryoObject the Gamebryo object equivalent.
        */
        void AddConverted(CSLModel& in_XSIModel, NiAVObject& in_GamebryoObject);

        /** Add a new mapping between a Gamebryo object and the top Gamebryo node
            used to represent its transform.
            @param in_GamebryoObject the Gamebryo object to set.
            @param in_TopObject the top object used for positioning.
        */
        void AddTransformTop(NiAVObject& in_GamebryoObject, NiAVObject& in_TopObject);

        /** Lookup the Gamebryo object corresponding to an XSI model in the map.
            @param CSLModel& the XSI model we know.
            @return the Gamebryo NiAVObject, if any. May return NULL.
        */
        NiAVObject * FindObject(CSLModel& in_XSIModel) const;

        /** Lookup the Gamebryo object corresponding to the top transform node of an object.
            @param in_GamebryoObject the Gamebryo object to set.
            @return the Gamebryo NiAVObject, if any. May return NULL.
        */
        NiAVObject * FindTransformTop(const NiAVObject& in_GamebryoObject) const;

        /** Retrieve the Gamebryo node corresponding to the parent of an XSI model.
            @param CSLModel& the XSI model.
            @return the Gamebryo NiNode corresponding to the parent of the XSI model. May be NULL.
        */
        NiNode * FindParentNode(CSLModel& in_XSIModel) const;

        /** Retrieve a node by name using recursive search of roots.
            @param in_Name the name of the node to find.
            @return the Gamebryo node or NULL if not found.
        */
        NiNode* FindNodeRecursive(const NiFixedString& in_Name) const;

        /** Add a root Gamebryo object. This is an object without parent.
            @param NiAVObject* the Gamebryo root object.
        */
        void AddRoot(NiAVObject& in_pGamebryoObject);

        /** Retrieve the list of Gamebryo root objects.
            @return the list of roots.
        */
        RootObjects & GetRoots();
    
        /** Retrievea LOD (levels of details) node group. Creates the LOD node if needed.
            @param in_Name the name of the LOD group.
            @return the Gamebryo LOD switch node.
        */
        NiLODNode& GetLODGroup(const NiFixedString& in_Name);

        /** Set the scale associated with the model.
            @param CSLModel& the model owning the scale.
            @param CSIBCVector& the scale.
        */
        void SetModelScale(CSLModel & in_Model, const CSIBCVector3D & in_Scale);

        /* Retrieve the potential scale of a model.
           @param CSLModel& the model to look-up.
           @return the scale or NULL.
        */
        const CSIBCVector3D* FindModelScale(CSLModel & in_Model) const;

        /** Set the base pose of a Gamebryo object.
            @param in_GamebryoObject the Gamebryo object to set.
            @param in_BasePose the base pose as a vertex-to-world transform.
        */
        void SetBasePose(NiAVObject& in_GamebryoObject, NiAVObject& in_BasePose);

        /** Retrieve the potential base pose of a Gamebryo object.
            @param in_GamebryoObject the Gamebryo object to look-up.
            @return the hierarchy of nodes making up the base pose transform, if any, or NULL.
        */
        NiAVObject* FindBasePose(const NiAVObject& in_GamebryoObject) const;

    private:
        // Prevent copies.
        NodeMap(const NodeMap &);
        void operator=(const NodeMap &);

        typedef std::map<CSLModel*,NiAVObjectPtr>           GBObjects;
        typedef std::map<const NiAVObject*,NiAVObjectPtr>   GBObjectTops;
        typedef std::map<CSLModel*,CSIBCVector3D>           ModelScales;
        typedef std::map<NiFixedString,NiLODNodePtr>        LODNodes;
        typedef std::map<const NiAVObject*,NiAVObjectPtr>   BasePoses;

        GBObjects               m_GBObjects;
        GBObjectTops            m_GBObjectTops;
        ModelScales             m_ModelScales;
        RootObjects             m_RootObjects;
        LODNodes                m_LODNodes;
        BasePoses               m_BasePoses;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_NODE_MAP_H */
