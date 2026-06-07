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

#ifndef NIMAXCAMERACONVERTER_H
#define NIMAXCAMERACONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"

//---------------------------------------------------------------------------

class NiMAXCameraConverter
{
public:

    NiMAXCameraConverter(TimeValue animStart, TimeValue animEnd);
    ~NiMAXCameraConverter() {};

    // the CameraInfo is a struct MI uses to keep
    // track of a scene graph's cameras. It has
    // NiCamera (pCam), the NiNode used to move
    // the camera around (pCamNode) and whether
    // the hither & yon fields are fix or can
    // vary (bCameraFixed)
    class CameraInfo : public NiMemObject
    {
    public:
        CameraInfo();
        ~CameraInfo();

        NiCameraPtr spCam;
        NiNodePtr   spCamNode;
        bool      bCameraFixed;
        bool      bIsCurrent;
    };

    int Convert(Interface *pIntf,
                INode *pNode,
                Object *pObj, 
                NiNode *pCurNode, 
                TimeValue animStart);
    static int ConvertDefaultCamera(Interface* pIntf, NiNode* pRoot, 
                                    CameraInfo* pResult, float fAnimStart,
                                    NiPoint3& offset);

    inline static void SetConvertCameras(bool bConvertCameras)
                    { m_bConvertCameras = bConvertCameras; }
    inline static bool GetConvertCameras() { return m_bConvertCameras; }

    inline static NiTPointerList<CameraInfo *> &GetCamList() { return m_camList; }

    static void Init();
    static void Shutdown();

private:
    TimeValue m_animStart, m_animEnd;

    static bool m_bConvertCameras;
    static NiTPointerList<CameraInfo *> m_camList;

    static void *m_pUserData;

};
//---------------------------------------------------------------------------


#endif
