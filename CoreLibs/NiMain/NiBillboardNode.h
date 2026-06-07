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

#ifndef NIBILLBOARDNODE_H
#define NIBILLBOARDNODE_H

#include "NiNode.h"
#include "NiStream.h"
class NiCamera;

class NIMAIN_ENTRY NiBillboardNode : public NiNode
{
    NiDeclareRTTI;
    NiDeclareClone(NiBillboardNode);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    // construction
    NiBillboardNode();

    // mode selection
    enum FaceMode
    {
        ALWAYS_FACE_CAMERA,
        ROTATE_ABOUT_UP,
        RIGID_FACE_CAMERA,
        ALWAYS_FACE_CENTER,
        RIGID_FACE_CENTER
    };

    inline void SetMode(FaceMode eMode);
    FaceMode GetMode() const;

    // *** begin Emergent internal use only ***

    // updates
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateWorldBound();

    static char* GetViewerString(const char* pcPrefix, FaceMode eMode);

    // *** end Emergent internal use only ***

protected:
    void RotateToCamera(const NiCamera* pkCamera);
    bool RotateToCenter(const NiCamera* pkCamera,
        const NiPoint3& kWorldTranslate, NiPoint3& kCamD, NiPoint3& kCamU,
        NiPoint3& kCamR);

    // culling pass (separate from display pass)
    virtual void OnVisible(NiCullingProcess& kCuller);

    inline void SetUpdateControllers(bool bUpdateControllers);
    inline bool GetUpdateControllers() const;

    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 11
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        FACE_MODE_MASK          = 0x0007,
        FACE_MODE_POS           = 0,
        UPDATECONTROLLERS_MASK  = 0x0008
    };
};

typedef NiPointer<NiBillboardNode> NiBillboardNodePtr;

#include "NiBillboardNode.inl"

#endif
