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

#ifndef NISHADOWCLICKVALIDATOR_H
#define NISHADOWCLICKVALIDATOR_H

#include "NiRenderClickValidator.h"
#include "NiCamera.h"
#include "NiCullingProcess.h"

class NIMAIN_ENTRY NiShadowClickValidator : public NiRenderClickValidator
{
    NiDeclareRTTI;

public:
    // Returns whether or not to execute the specified render click for the
    // specified frame.
    virtual bool ValidateClick(NiRenderClick* pkRenderClick,
        unsigned int uiFrameID);

    inline void SetCamera(NiCamera* pkCamera);
    inline NiCamera* GetCamera() const;

    inline void SetCullingProcess(NiCullingProcess* pkCullingProcess);
    inline NiCullingProcess* GetCullingProcess() const;

protected:
    NiCameraPtr m_spCamera;
    NiCullingProcessPtr m_spCullingProcess;

    NiVisibleArray m_kVisibleSet;
};

NiSmartPointer(NiShadowClickValidator);

#include "NiShadowClickValidator.inl"

#endif  // #ifndef NISHADOWCLICKVALIDATOR_H
