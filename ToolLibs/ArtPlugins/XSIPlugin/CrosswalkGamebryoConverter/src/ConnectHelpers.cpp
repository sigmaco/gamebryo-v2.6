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

#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoNodeMap.h"

#include "Model.h"
#include "CustomPSet.h"

#include "NiNode.h"
#include "NiLODNode.h"
#include "NiRangeLODData.h"

namespace
{
    //---------------------------------------------------------------------------
    // Rebuild the array of child node of the LOD switcher to be in distance order.
    //---------------------------------------------------------------------------
    void SortByDistance(
        NiLODNode& io_LODNode,
        NiRangeLODData& io_RangeData,
        float distance,
        NiAVObject& io_TopChild)
    {
        static const float MAX_FAR_DISTANCE = 1.0e20f;

        // Save all known range/node relations. We use a map so that
        // the node will be ordered by distance.
        typedef std::map<float,NiAVObjectPtr> NodeDistances;
        NodeDistances nodeDistances;
        nodeDistances[distance] = &io_TopChild;
        NIASSERT(distance < MAX_FAR_DISTANCE);
        nodeDistances[MAX_FAR_DISTANCE]  = 0;
        for (unsigned int i = 0; i < io_LODNode.GetChildCount(); ++i)
        {
            float nearDistance, farDistance;
            io_RangeData.GetRange(i, nearDistance, farDistance);
            nodeDistances[nearDistance] = io_LODNode.GetAt(i);
        }

        // Resets the LOD switcher.
        io_LODNode.RemoveAllChildren();
        io_RangeData.SetNumRanges(nodeDistances.size() - 1);

        // Re-insert the different LOD in distance order.
        typedef NodeDistances::iterator iter;
        int index = 0;
        iter next = nodeDistances.begin();
        for (iter pos = next++; next != nodeDistances.end(); ++pos, ++next, ++index)
        {
            io_LODNode.AttachChild(pos->second);
            io_RangeData.SetRange(index, pos->first, next->first);
        }
    }

    //---------------------------------------------------------------------------
    // Insert the LOD node if needed. Also updates the LOD data so that the
    // LOD node are in the proper distance order for the range LOD logic.
    //---------------------------------------------------------------------------
    NiAVObject& InsertLODIfNeeded(
        epg::Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_TopChild)
    {
        // If we need to support LOD, we add the LOD switch node above
        // whatever was done.
        CSLCustomPSet* lodProps = epg::FindCustomProperties(
            in_Model,
            XSI_GAMEBRYO_LOD_PROPERTY_NAME);
        if (!lodProps)
            return io_TopChild;

        CSIBCString lodGroupName;
        if (!epg::FindStringParameter(
            *lodProps,
            XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME,
            lodGroupName))
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "The Gamebryo level-of-Details property in \"%s\""
                " is missing the group name parameter, ignoring LOD.",
                in_Model.Name().GetText());
            return io_TopChild;
        }

        float lodDistance;
        if (!epg::FindFloatParameter(
            *lodProps,
            XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME,
            lodDistance))
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "The Gamebryo level-of-Details property in \"%s\""
                " is missing the distance parameter, ignoring LOD.",
                in_Model.Name().GetText());
            return io_TopChild;
        }

        NiLODNode& lodNode = io_Context.GetNodeMap().GetLODGroup(lodGroupName.GetText());
        NiLODData* lodData = lodNode.GetLODData();
        if (!lodData)
            return io_TopChild;

        NiRangeLODData* rangeData = NiDynamicCast(NiRangeLODData, lodData);
        if (!rangeData)
            return io_TopChild;

        SortByDistance(lodNode, *rangeData, lodDistance, io_TopChild);

        return lodNode;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Connect the node to its parent. Handles the fact that an XSI model
    // can be converted to a mini-tree of nodes and handles the level-of-details
    // switching.
    //---------------------------------------------------------------------------
    bool ConnectNodes(
        Context & io_Context,
        CSLModel & in_Model,
        NiAVObject& io_TopChild,
        NiAVObject& io_BottomChild)
    {
        NodeMap& nodeMap = io_Context.GetNodeMap();
        NiAVObject& topWithLOD = InsertLODIfNeeded(io_Context, in_Model, io_TopChild);
        NiNode*  parent  = nodeMap.FindParentNode(in_Model);
        if (parent)
            parent->AttachChild(&topWithLOD);
        else
            nodeMap.AddRoot(topWithLOD);
        nodeMap.AddConverted(in_Model, io_BottomChild);
        nodeMap.AddTransformTop(io_BottomChild, topWithLOD);

        return true;
    }

    //---------------------------------------------------------------------------
}
