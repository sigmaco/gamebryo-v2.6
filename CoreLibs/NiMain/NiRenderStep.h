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

#ifndef NIRENDERSTEP_H
#define NIRENDERSTEP_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiRTTI.h"
#include "NiFixedString.h"

class NiRenderTargetGroup;

class NIMAIN_ENTRY NiRenderStep : public NiRefObject
{
    NiDeclareRootRTTI(NiRenderStep);

public:
    NiRenderStep();

    // Functions for accessing the name of the render step.
    inline void SetName(const NiFixedString& kName);
    inline const NiFixedString& GetName() const;

    // Function that invokes callback functions and calls PerformRendering.
    inline void Render();

    // Active state accessors.
    inline void SetActive(bool bActive);
    inline bool GetActive() const;

    // Functions for getting and setting the render target group that will
    // contain the final output for this render step.
    virtual bool SetOutputRenderTargetGroup(
        NiRenderTargetGroup* pkOutputRenderTargetGroup) = 0;
    virtual NiRenderTargetGroup* GetOutputRenderTargetGroup() = 0;

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const = 0;
    virtual float GetCullTime() const = 0;
    virtual float GetRenderTime() const = 0;

    // Callback definition.
    typedef bool (*Callback)(NiRenderStep* pkCurrentStep,
        void* pvCallbackData);

    // Pre-processing callback.
    inline void SetPreProcessingCallbackFunc(Callback pfnCallback,
        void* pvCallbackData = NULL);
    Callback GetPreProcessingCallbackFunc() const;
    inline void* GetPreProcessingCallbackFuncData() const;

    // Post-processing callback.
    inline void SetPostProcessingCallbackFunc(Callback pfnCallback,
        void* pvCallbackData = NULL);
    Callback GetPostProcessingCallbackFunc() const;
    inline void* GetPostProcessingCallbackFuncData() const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    // Function that actually performs the rendering for the render step.
    virtual void PerformRendering() = 0;

    // Callback functions.
    Callback m_pfnPreProcessingCallback;
    Callback m_pfnPostProcessingCallback;

    // Callback data.
    void* m_pvPreProcessingCallbackData;
    void* m_pvPostProcessingCallbackData;

    // The name of the render step.
    NiFixedString m_kName;

    // Whether or not the render step is active.
    bool m_bActive;

    // Default name for all render steps.
    static NiFixedString ms_kDefaultName;
};

NiSmartPointer(NiRenderStep);

#include "NiRenderStep.inl"

#endif  // #ifndef NIRENDERSTEP_H
