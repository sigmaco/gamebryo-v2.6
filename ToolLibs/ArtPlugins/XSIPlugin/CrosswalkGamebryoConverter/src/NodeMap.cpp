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

#include "CrosswalkGamebryoNodeMap.h"

#include "Model.h"

#include "NiNode.h"
#include "NiLODNode.h"
#include "NiRangeLODData.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Create an empty node map.
    //---------------------------------------------------------------------------
    NodeMap::NodeMap()
    {
        // Do nothing.
    }

    //---------------------------------------------------------------------------
    // Empty the node map.
    //---------------------------------------------------------------------------
    void NodeMap::Clear()
    {
        m_GBObjects.clear();
        m_GBObjectTops.clear();
        m_ModelScales.clear();
        m_RootObjects.clear();
        m_LODNodes.clear();
        m_BasePoses.clear();
    }

    //---------------------------------------------------------------------------
    // Reset all objects to be under this root.
    //---------------------------------------------------------------------------
    void NodeMap::Transplant(NiNode& in_SceneRoot)
    {
        typedef RootObjects::iterator iter;
        for (iter pos = m_RootObjects.begin(); pos != m_RootObjects.end(); ++pos)
        {
            if (&in_SceneRoot != *pos)
            {
                in_SceneRoot.AttachChild(*pos);
            }
        }

        m_RootObjects.clear();
        m_RootObjects.push_back(&in_SceneRoot);
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between an XSI model and a Gamebryo object.
    //---------------------------------------------------------------------------
    void NodeMap::AddConverted(CSLModel & in_XSIModel, NiAVObject & in_GBObject)
    {
        // Note: we allow adding the same XSI model more than once to support
        //       case were the code need to "reconnect" the mapping to the
        //       Gamebryo object. This allows working with intermediary dummy
        //       nodes using common code and then reconnect the node as needed.
        m_GBObjects[&in_XSIModel] = &in_GBObject;
    }

    //---------------------------------------------------------------------------
    // Add a new mapping between a Gamebryo object and the top Gamebryo node
    // used to represent its transform.
    //---------------------------------------------------------------------------
    void NodeMap::AddTransformTop(NiAVObject& in_GamebryoObject, NiAVObject & in_TopObject)
    {
        // Note: we allow adding the same Gamerbryo object more than once to support
        //       case were the code need to "reconnect" the mapping to the
        //       Gamebryo object. This allows working with intermediary dummy
        //       nodes using common code and then reconnect the node as needed.
        m_GBObjectTops[&in_GamebryoObject] = &in_TopObject;
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo object corresponding to an XSI model in the map.
    //---------------------------------------------------------------------------
    NiAVObject* NodeMap::FindObject(CSLModel& in_XSIModel) const
    {
        typedef GBObjects::const_iterator iter;
        const iter pos = m_GBObjects.find(&in_XSIModel);
        if (pos != m_GBObjects.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Lookup the Gamebryo object corresponding to the top transform node of a Gamebryo object.
    //---------------------------------------------------------------------------
    NiAVObject* NodeMap::FindTransformTop(const NiAVObject& in_GamebryoObject) const
    {
        typedef GBObjectTops::const_iterator iter;
        const iter pos = m_GBObjectTops.find(&in_GamebryoObject);
        if (pos != m_GBObjectTops.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the Gamebryo node corresponding to the parent of an XSI model.
    //---------------------------------------------------------------------------
    NiNode* NodeMap::FindParentNode(CSLModel& in_XSIModel) const
    {
        CSLModel * parentModel = in_XSIModel.Parent();
        if (!parentModel)
            return NULL;

        NiAVObject * parent = FindObject(*parentModel);
        if (!parent)
            return NULL;
        
        return NiDynamicCast(NiNode,parent);
    }

    //---------------------------------------------------------------------------
    // Retrieve a node by name using recursive search of roots.
    //---------------------------------------------------------------------------
    NiNode* NodeMap::FindNodeRecursive(const NiFixedString& in_Name) const
    {
        typedef RootObjects::const_iterator iter;
        for (iter pos = m_RootObjects.begin(); pos != m_RootObjects.end(); ++pos)
            if (NiAVObject* pNode = (*pos)->GetObjectByName(in_Name))
                return NiDynamicCast(NiNode, pNode);

        return NULL;
    }

    //---------------------------------------------------------------------------
    // Add a root Gamebryo object. This is an object without parent.
    //---------------------------------------------------------------------------
    void NodeMap::AddRoot(NiAVObject& in_GamebryoObject)
    {
        m_RootObjects.push_back(&in_GamebryoObject);
    }

    //---------------------------------------------------------------------------
    // Retrieve the list of Gamebryo root objects.
    //---------------------------------------------------------------------------
    NodeMap::RootObjects& NodeMap::GetRoots()
    {
        return m_RootObjects;
    }

    //---------------------------------------------------------------------------
    // Retrieve a LOD (levels of details) node group. Creates the LOD node if needed.
    //---------------------------------------------------------------------------
    NiLODNode& NodeMap::GetLODGroup(const NiFixedString& in_Name)
    {
        NiLODNodePtr& lodNode = m_LODNodes[in_Name];
        if (!lodNode)
        {
            lodNode = NiNew NiLODNode;
            // Note: NiLODNode takes ownership of the NiRangeLODData.
            lodNode->SetLODData(NiNew NiRangeLODData);
            lodNode->SetLODActive(true);
        }
        return *lodNode;
    }

    //---------------------------------------------------------------------------
    // Set the scale associated with the model.
    //---------------------------------------------------------------------------
    void NodeMap::SetModelScale(CSLModel& in_Model, const CSIBCVector3D& in_Scale)
    {
        m_ModelScales[&in_Model] = in_Scale;
    }

    //---------------------------------------------------------------------------
    // Retrieve the potential scale of a model.
    //---------------------------------------------------------------------------
    const CSIBCVector3D* NodeMap::FindModelScale(CSLModel& in_Model) const
    {
        typedef ModelScales::const_iterator iter;
        const iter pos = m_ModelScales.find(&in_Model);
        if (pos != m_ModelScales.end())
            return &(pos->second);
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
    // Set the base pose of a Gamebryo object.
    //---------------------------------------------------------------------------
    void NodeMap::SetBasePose(NiAVObject& in_GamebryoObject, NiAVObject& in_BasePose)
    {
        m_BasePoses[&in_GamebryoObject] = &in_BasePose;
    }

    //---------------------------------------------------------------------------
    // Retrieve the potential base pose of a Gamebryo object.
    //---------------------------------------------------------------------------
    NiAVObject* NodeMap::FindBasePose(const NiAVObject& in_GamebryoObject) const
    {
        typedef BasePoses::const_iterator iter;
        const iter pos = m_BasePoses.find(&in_GamebryoObject);
        if (pos != m_BasePoses.end())
            return pos->second;
        else
            return NULL;
    }

    //---------------------------------------------------------------------------
}
