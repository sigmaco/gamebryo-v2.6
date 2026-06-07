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

#pragma once
#include "MSceneObject.h"

namespace NiManagedToolInterface
{
    public __gc class MCamera
    {
    public:
        MCamera();
        ~MCamera();
        void DeleteContents();

        __property bool get_Active();
        __property int get_ViewWidth();
        __property void set_ViewWidth(int iWidth);
        __property int get_ViewHeight();
        __property void set_ViewHeight(int iHeight);

        __value enum UpAxisType : unsigned int
        {        
            Z_AXIS = 0,
            Y_AXIS = 1
        };

        __property UpAxisType get_UpAxis();
        __property void set_UpAxis(UpAxisType eType);

        __property bool get_InvertPan();
        __property void set_InvertPan(bool bInvert);

        NiCamera* GetCamera();
        
        void CalcCameraTransform(NiAVObject* pkScene);
        void CalcCameraFrustum(NiAVObject* pkScene);

        void Zoom(float fMultiplier, NiPoint3 kZoomToPoint, 
            NiBound kZoomToBound);
        void ZoomExtents();
        void CenterToScene(NiAVObject* pkScene);


        void RotateX(float fXRot);
        void RotateUp(float fUpRot);
        void SetTranslate(NiPoint3 kTranslate);

        void Reset();
        bool WindowPointToRay(int iX, int iY, NiPoint3& origin, NiPoint3& dir);

        void Update(float fTime);

        void GetWorldTranslate(NiPoint3& kTranslate);
        void GetWorldRotate(NiMatrix3& kRotate);
    protected:
        int m_iWidth;
        int m_iHeight;

        NiNode* m_spCamRootNode;
        NiNode* m_pkCamXRot;
        NiNode* m_pkCamZRot;
        NiNode* m_pkCameraNode;
        NiCamera* m_pkCamera;
        UpAxisType m_eUpAxis;
        bool m_bInvertPan;
    };
}
