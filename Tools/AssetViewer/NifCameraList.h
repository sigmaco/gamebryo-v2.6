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

// NifCameraList.h

#ifndef NIFCAMERALIST_H
#define NIFCAMERALIST_H

class CNifDoc;

class NiCameraInfo  : public NiMemObject
{
    public:
        NiCameraPtr m_spCam;
        NiCameraPtr m_spOriginalCam;
        NiNodePtr m_spOriginalParent;
        bool m_bIsAnimated;
        NiMatrix3 m_kOriginalRotation;
        NiPoint3 m_kOriginalTranslation;
        bool m_bIsDefault;

        static const char* ms_kDefaultCameraName;
};

struct NiCameraInfoIndex
{
    unsigned int uiRootIndex;
    unsigned int uiCameraIndex;
};

class CNifCameraList : public NiMemObject
{
    public:
        CNifCameraList(CNifDoc* pkDoc);
        ~CNifCameraList();

        // Camera data accessors
        NiCamera* GetCamera(NiCameraInfoIndex kIndex);
        NiCameraInfo* GetCameraInfo(NiCameraInfoIndex kIndex);
        NiCamera* GetCameraFromOriginal(NiCamera* pkCamera) const;

        NiCameraInfoIndex GetNextCameraInfoIndex(NiCameraInfoIndex kIndex);
        NiCameraInfoIndex GetPreviousCameraInfoIndex(NiCameraInfoIndex kIndex);

        unsigned int NumCameras();
        unsigned int NumCamerasForRoot(unsigned int uiWhichRoot);
        
        void AddCamerasForRoot(NiNode* pkRoot);
    
        void CreateCameraList();
        void EmptyCameraList();
        
        void FindAndAddCameras(unsigned int uiWhichRoot, NiNode* pkNode);
        void RemoveCamerasForRoot(NiNode* pkRoot, bool bCompact = true);
        NiCameraInfoIndex AddCamera(unsigned int uiWhichRoot,
            NiCamera* pkCamera);
        void RemoveCamera(unsigned int uiWhichRoot, NiCamera* pkCamera);

    protected:
        NiTPrimitiveArray<NiTPrimitiveArray<NiCameraInfo*>*> m_pkCameraList;
        CNifDoc* m_pkDoc;
};

#endif  // #ifndef NIFCAMERALIST_H
