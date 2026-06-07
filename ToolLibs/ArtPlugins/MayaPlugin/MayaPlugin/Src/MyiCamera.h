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

#ifndef MYICAMERA_H
#define MYICAMERA_H

#include <NiSmartPointer.h>
#include <windows.h>

NiSmartPointer(NiCamera);

class MyiCamera
{
    public:
        MyiCamera();
        MyiCamera(int iCameraID);
        MyiCamera::MyiCamera(NiCameraPtr pNiCamera);
        ~MyiCamera();
        inline NiCamera *GetCamera();
        
        int m_iCameraID;

    protected:

        void SetTransformation();
        void SetProjection();

        NiCameraPtr m_spCamera;
};
//---------------------------------------------------------------------------
inline NiCamera *MyiCamera::GetCamera()
{
    return m_spCamera;
}
//---------------------------------------------------------------------------

#endif // MYICAMERA_H
