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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoMeshHelpers.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoLightingMap.h"

#include "Model.h"
#include "XSIMesh.h"
#include "XSIShape.h"
#include "XSITriangleList.h"

#include "NiNode.h"

namespace
{
    //---------------------------------------------------------------------------
    // Read the lighting property and record it in the lighting map.
    //---------------------------------------------------------------------------
    bool ConvertLightingProperty(
        epg::Context & io_Context,
        CSLTemplate& in_Model,
        NiNode& in_Node)
    {
        std::vector<NiFixedString> tags;
        std::vector<NiFixedString> ignoredCasters;
        std::vector<NiFixedString> ignoredReceivers;
        if (epg::LoadShadowProperty(in_Model, tags, ignoredCasters, ignoredReceivers))
        {
            epg::LightingMap& lightingMap = io_Context.GetLightingMap();

            typedef std::vector<NiFixedString>::iterator iter;

            for (iter pos = tags.begin(); pos != tags.end(); ++pos)
                if (pos->GetLength() > 0)
                    io_Context.GetLightingMap().AddLitObject(*pos, in_Node);

            for (iter pos = ignoredCasters.begin(); pos != ignoredCasters.end(); ++pos)
                if (pos->GetLength() > 0)
                    io_Context.GetLightingMap().DontCastShadows(*pos, in_Node);

            for (iter pos = ignoredReceivers.begin(); pos != ignoredReceivers.end(); ++pos)
                if (pos->GetLength() > 0)
                    io_Context.GetLightingMap().DontReceiveShadow(*pos, in_Node);
        }

        return true;
    }
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert meshes.
    //---------------------------------------------------------------------------

    bool ConvertMesh(Context& io_Context, CSLModel& in_Model)
    {
        return io_Context.Logf(
            LOG_ERROR,
            "XSI mesh \"%s\" use unsupported old format.",
            in_Model.Name().GetText());
    }

    //---------------------------------------------------------------------------
    bool ConvertXSIMesh(Context& io_Context, CSLModel& in_Model)
    {
        // Note: the following code is inspired by cnv_mesh.cpp
        // in dosXSIConverter example code included in the
        // XSI Crosswalk SDK.

        CSLXSIMesh* meshXSI = static_cast<CSLXSIMesh *>(in_Model.Primitive());
        if (!meshXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI mesh \"%s\".",
                in_Model.Name().GetText());
        }

        CSLXSIShape* shape = meshXSI->XSIShape();
        if (!shape)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "XSI mesh \"%s\" without shape.",
                in_Model.Name().GetText());
        }

        CSLXSIShapeAnimation* shapeAnim = meshXSI->XSIShapeAnimation();

        // Convert the shape attributes to simpler-to-access maps.
        StandardSemanticMapper semanticMap(StandardSemanticMapper::STANDARD_MODE);
        ShapeAttributeMap attrMap(*shape, semanticMap);

        
        const SI_Int triangleListCount = meshXSI->GetXSITriangleListCount();
        if (triangleListCount > 0)
        {
            // There are many reasons to create a dummy node above the mesh:
            //
            //   - If the mesh has children then we need to insert a node
            //     above the mesh that will contain the children since
            //     Gamebryo's meshes are leaves in the tree while XSI meshes
            //     can have children.
            //
            //   - If there is more than one triangle list then creating
            //     a node allows using the same base transformation for
            //     all meshes.
            //
            //   - If there is LOD (levels of details) with multiple
            //     alternative meshes then we need to insert a NiSwitchNode
            //     anyway.
            //
            // Thus, for simplicity, we always create a dummy node. The potential
            // overhead is eliminated by Gamebryo's scene optimizer plugin which
            // can remove unnecessary nodes as needed.

            NiNodePtr pkNode = NiNew NiNode;
            if (!ConvertCommonProperties(io_Context, in_Model, *pkNode, in_Model.Name()))
                return false;

            if (!ConvertLightingProperty(io_Context, in_Model, *pkNode))
                return false;

            // Note: we are converting each triangle list into a separate mesh.
            //       This means that we assume that shape animations will always
            //       morph between shapes with the same number of triangle lists
            //       which divide the total geometry in the same way.
            //
            //       While this is a very reasonable assumption, this may need
            //       to be modified in the future.
            //
            //       Also, it would arguably be more efficient for the renderer
            //       to provide each triangle list as sub-meshes of the same
            //       meshes (at least the Gamebryo documentation suggest it.)
            //       On the other hand, the plugin pipeline can optimize meshes
            //       so complexifying the code here is probablyh useless from
            //       a performance point-of-view and would make the code harder
            //       to understand.
            //
            //       Thus the current situation stands. Use mesh optimizers
            //       if needed when converting.
            CSLXSITriangleList** triangleListArray = meshXSI->XSITriangleLists();
            for (SI_Int i = 0; i < triangleListCount; ++i)
            {
                CSLXSITriangleList* triangles = triangleListArray[i];
                if (!triangles)
                    return false;

                NiMesh* pMesh = ConvertTriangleListToMesh(
                                    io_Context,
                                    in_Model,
                                    *shape,
                                    attrMap,
                                    *triangles,
                                    *pkNode);

                if (!pMesh)
                    return false;

                if (!ConvertSkin(io_Context, in_Model, *shape, *triangles, *pMesh))
                    return false;

                if (shapeAnim)
                {
                    if (!ConvertShapeAnimation(
                        io_Context,
                        in_Model,
                        *shape,
                        *shapeAnim,
                        *triangles,
                        *pMesh))
                    {
                        return false;
                    }
                }
            }
        }

        NIASSERT(meshXSI->GetXSITriangleStripListCount() == 0);

        return true;
    }
    //---------------------------------------------------------------------------
}
