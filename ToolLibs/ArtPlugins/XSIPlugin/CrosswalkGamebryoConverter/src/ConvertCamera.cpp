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
#include "CrosswalkGamebryoSettings.h"
#include "CrosswalkGamebryoImpl.h"

#include "Model.h"
#include "XSICamera.h"

#include "NiCamera.h"
#include "NiNode.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert cameras.
    //---------------------------------------------------------------------------

    bool ConvertXSICamera(Context& io_Context, CSLModel& in_Model)
    {
        CSLXSICamera* cameraXSI = static_cast<CSLXSICamera *>(in_Model.Primitive());
        if (!cameraXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI camera \"%s\".",
                in_Model.Name().GetText());
        }

        if (io_Context.GetSettings().GetExportCamera())
        {
            NiCameraPtr cameraGB = NiNew NiCamera;

            // Due to the differences between how the camera is positined by default
            // (looking down Z+ in XSI, looking down X+ in Gamebryo) we need to
            // create a node above the camera so that:
            //
            //    - The camera can be first rotated 90 degree around its Y axis
            //      to remap the X into the Z, in Gamebryo.
            //
            //    - The XSI-requested rotation can be applied on top and after
            //      that rotation.
            //
            // Furthermore, due to the change in axis, the rotation order must
            // be changed from XYZ to ZYX.
            {
                if (!ConvertCommonProperties(io_Context, in_Model, *cameraGB, true))
                    return false;
                NiTransform transform = cameraGB->GetLocalTransform();
                transform.m_Rotate.FromEulerAnglesXYZ(0,DegreeToRadian(-90),0);
                cameraGB->SetLocalTransform(transform);
            }

            // Since the view plane is by definition (see Gamebryo docs) at distance
            // 1 (one) from the camera, and the field of view, angle A, by definition,
            // extends as much left and right, then the right-most position (P) of the
            // view at the view plane is:
            //
            //   P = sin(A / 2) * hypothenuse               [1]
            //
            // But we only know the distance to the view screen to be 1 (one).
            // So we use:
            //
            //   tan(x) = sin(x) / cos(x)
            //   sin(x) = tan(x) * cos(x)                   [2]
            //
            // We know:
            //
            //   cos(x) * hypethenuse == 1                  [3]
            //
            // So:
            //
            //   P = sin(A / 2) * hypothenuse               [1]
            //   P = tan(A / 2) * cos(A / 2) * hypothenuse  [replace using [2]]
            //   P = tan(A / 2) * 1                         [replace using [3]]
            //   P = tan(A / 2)

            const float tangent = std::tan(DegreeToRadian(cameraXSI->GetFieldOfView()/2));
            float left   = -tangent;
            float right  =  tangent;
            float top    =  tangent;
            float bottom = -tangent;
            bool isFovHoriz = (cameraXSI->GetFieldOfViewType() != 0);
            const float aspect = cameraXSI->GetPictureRatio();
            if (isFovHoriz)
            {
                top    /= aspect;
                bottom /= aspect;
            }
            else
            {
                left  /= aspect;
                right /= aspect;
            }
            const float nearPlane = cameraXSI->GetNearPlane();
            const float farPlane  = cameraXSI->GetFarPlane();
            const bool isOrtho = (cameraXSI->GetProjectionPlaneEnable() != 0);
            NiFrustum frustum(left, right, top, bottom, nearPlane, farPlane, isOrtho);
            cameraGB->SetViewFrustum(frustum);
        }
        else
        {
            // If the camera has children, then we need to create a dummy node.
            if (in_Model.GetChildrenCount() > 0)
            {
                NiNode* pkNode = NiNew NiNode;
                if (!ConvertCommonProperties(io_Context, in_Model, *pkNode, in_Model.Name()))
                    return false;
            }
        }

        return true;
    }
    //---------------------------------------------------------------------------
}
