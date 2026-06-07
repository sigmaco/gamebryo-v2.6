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

#ifndef NICULLEDOBJECTVALIDATOR_H
#define NICULLEDOBJECTVALIDATOR_H

#include "NiRenderClickValidator.h"
#include "NiCullingProcess.h"

class NIMAIN_ENTRY NiCulledObjectValidator : public NiRenderClickValidator
{
    NiDeclareRTTI;

public:
    NiCulledObjectValidator(NiAVObject* pkObject, NiCamera* pkCamera,
        NiCullingProcess* pkCullingProcess, bool bReturnOnCulled = false);

    // Returns whether or not to execute the specified render click for the
    // specified frame.
    virtual bool ValidateClick(NiRenderClick* pkRenderClick,
        unsigned int uiFrameID);

    // Functions for accessing the object to test.
    inline void SetObject(NiAVObject* pkObject);
    inline NiAVObject* GetObject() const;

    // Functions for accessing the camera to use.
    inline void SetCamera(NiCamera* pkCamera);
    inline NiCamera* GetCamera() const;

    // Functions for accessing the culling process to use when culling.
    inline void SetCullingProcess(NiCullingProcess* pkCullingProcess);
    inline NiCullingProcess* GetCullingProcess() const;

    // Functions for setting the value that will be returned if the object is
    // culled.
    inline void SetReturnOnCulled(bool bReturnOnCulled);
    inline bool GetReturnOnCulled() const;

protected:
    // The object to test.
    NiAVObjectPtr m_spObject;

    // The camera to use when culling.
    NiCameraPtr m_spCamera;

    // The culling process to use when culling.
    NiCullingProcessPtr m_spCullingProcess;

    // An array to hold the culling results.
    NiVisibleArray m_kVisibleSet;

    // Cached values for the last frame ID and return value.
    unsigned int m_uiLastFrameID;
    bool m_bLastReturnValue;

    // The value to return if the object is culled.
    bool m_bReturnOnCulled;

    // A flag indicating whether or not the next call to ValidateClick should
    // re-perform the culling operation.
    bool m_bForceUpdate;
};

NiSmartPointer(NiCulledObjectValidator);

#include "NiCulledObjectValidator.inl"

#endif  // #ifndef NICULLEDOBJECTVALIDATOR_H
