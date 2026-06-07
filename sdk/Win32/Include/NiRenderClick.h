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

#ifndef NIRENDERCLICK_H
#define NIRENDERCLICK_H

#include "NiRenderClickValidator.h"
#include "NiRenderTargetGroup.h"
#include "NiRect.h"
#include "NiFixedString.h"
#include "NiColor.h"

class NIMAIN_ENTRY NiRenderClick : public NiRefObject
{
    NiDeclareRootRTTI(NiRenderClick);
    NiDeclareFlags(unsigned short);

public:
    NiRenderClick();

    // Functions for accessing the name of the render click.
    inline void SetName(const NiFixedString& kName);
    inline const NiFixedString& GetName() const;

    // Function that invokes callback functions and calls PerformRendering.
    void Render(unsigned int uiFrameID);

    // Uses render click validator to determine whether or not this render
    // click should be executed for the specified frame.
    inline bool ShouldPerformRendering(unsigned int uiFrameID);

    // Functions for specifying the render click validator.
    inline void SetValidator(NiRenderClickValidator* pkValidator);
    inline NiRenderClickValidator* GetValidator() const;

    // Functions for specifying the render target group.
    inline void SetRenderTargetGroup(NiRenderTargetGroup* pkRenderTargetGroup);
    inline NiRenderTargetGroup* GetRenderTargetGroup() const;

    // Functions for specifying buffers to be cleared.
    inline void SetClearAllBuffers(bool bClearAllBuffers);
    inline void SetClearColorBuffers(bool bClearBuffers);
    inline bool GetClearColorBuffers() const;
    inline void SetClearDepthBuffer(bool bClearBuffer);
    inline bool GetClearDepthBuffer() const;
    inline void SetClearStencilBuffer(bool bClearBuffer);
    inline bool GetClearStencilBuffer() const;

    // Functions for specifying a one-time clear.  These will override behavior
    // for a single frame, but the regular SetClear* take effect next frame
    void RequestClearAllBuffersOnce();
    void RequestClearColorBuffersOnce();
    void RequestClearDepthBufferOnce();
    void RequestClearStencilBufferOnce();

    // Functions for specifying the color to clear render target with.
    inline void SetBackgroundColor(const NiColorA& kColor);
    inline void GetBackgroundColor(NiColorA& kColor);
    inline void SetUseRendererBackgroundColor(bool bUseRendererBG);
    inline bool GetUseRendererBackgroundColor();
    inline void SetPersistBackgroundColorToRenderer(bool bPersist);
    inline bool GetPersistBackgroundColorToRenderer();

    // Functions for specifying the viewport.
    inline void SetViewport(const NiRect<float>& kViewport);
    inline const NiRect<float>& GetViewport() const;

    // Functions for accessing whether or not the render click is active.
    inline void SetActive(bool bActive);
    inline bool GetActive() const;

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const = 0;
    virtual float GetCullTime() const = 0;
    virtual float GetRenderTime() const = 0;

    // Callback definition.
    typedef bool (*Callback)(NiRenderClick* pkCurrentRenderClick,
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
    // flags
    enum
    {
        ACTIVE  = 0x0001,
        USE_RENDERER_BG_COLOR = 0x0002,
        PERSIST_BG_COLOR = 0x0004
    };

    // Function that actually performs the rendering for the render click.
    virtual void PerformRendering(unsigned int uiFrameID) = 0;

    // The validator used by the ShouldPerformRendering function.
    NiRenderClickValidatorPtr m_spValidator;

    // The render target group to use when rendering.
    NiRenderTargetGroupPtr m_spRenderTargetGroup;

    // The viewport to use when rendering.
    NiRect<float> m_kViewport;

    // Callback functions.
    Callback m_pfnPreProcessingCallback;
    Callback m_pfnPostProcessingCallback;

    // Callback data.
    void* m_pvPreProcessingCallbackData;
    void* m_pvPostProcessingCallbackData;

    // Bitfield that holds buffer clear mode.
    unsigned int m_uiClearMode;
    unsigned int m_uiSingleFrameClearMode;
    NiColorA m_kBackgroudColor;

    // The name of the render click.
    NiFixedString m_kName;

    // Default name for all render clicks.
    static NiFixedString ms_kDefaultName;
};

NiSmartPointer(NiRenderClick);

#include "NiRenderClick.inl"

#endif  // #ifndef NIRENDERCLICK_H
