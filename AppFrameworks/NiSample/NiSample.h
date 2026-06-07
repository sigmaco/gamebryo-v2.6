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


#ifndef NISAMPLE_H
#define NISAMPLE_H

// NiApplication framework
#include <NiApplication.h>
#include <NiCursor.h>
#include <NiUISlot.h>
#include <NiUILabel.h>
#include <NiUIRenderGroup.h>
#include <NiNavFlyController.h>
#include <NiNavOrbitController.h>
#include <NiShaderFactory.h>
#include <NiString.h>
#include <NiUIManagerRenderClick.h>
#include <NiDefaultClickRenderStep.h>
#include <NiCursorRenderClick.h>

class NiUIGroup;

class NiSample : public NiApplication
{
public:
    NiSample
    (
        const char* pcWindowCaption,            // Caption of window.
        unsigned int uiWidth = DEFAULT_WIDTH,   // Client window width.
        unsigned int uiHeight = DEFAULT_HEIGHT, // Client window height.
        bool bUseFrameSystem = false,           // Indicates whether or not to
                                                // use an NiRenderFrame to
                                                // render scene.
        unsigned int uiMenuID = 0,              // Menu resource ID.
        unsigned int uiNumStatusPanes = 0,      // Number of panes in status
                                                // window.
        unsigned int uiBitDepth = 0             // Color depth of screen.
    );

protected:
    
    virtual bool Initialize();
    virtual bool CreateInputSystem();
    virtual bool CreateCursor();
    virtual bool CreateUISystem();
    virtual bool CreateNavigationControllers();
    virtual bool AdjustNavigationControllers(NiAVObject* pkControlledObject,
        NiAVObject* pkReferenceObject, const NiPoint3& kUpDir, float fScale);
    virtual bool CreateUIElements();
    virtual bool CompleteUISystem();
    virtual bool CreateShaderSystem();

    // NiShader Functions
    virtual bool RunShaderParsers(const char* pcShaderDir);
    virtual bool RunShaderLibraries(const char* pcShaderDir);
    virtual bool RegisterShaderParsers();
    virtual bool RegisterShaderLibraries();

#if !defined(_USRDLL)
    static bool LibraryClassCreate(const char* pcLibFile,
        NiRenderer* pkRenderer, int iDirectoryCount,
        const char* apcDirectories[], 
        bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
    static unsigned int RunParser(const char* pcLibFile,
        NiRenderer* pkRenderer, const char* pcDirectory, 
        bool bRecurseSubFolders);
#endif

    static unsigned int ShaderErrorCallback(const char* pcError, 
        NiShaderError eError, bool bRecoverable);

    virtual bool CreateFrame();

    virtual void RenderFrame();
    virtual void RenderUIElements();

    // Destroy the font and user strings.
    virtual void Terminate();

    virtual void UpdateFrame();
    virtual void EndUpdate();

    virtual void UpdateInput();

    virtual void SetUseNavSystem(bool bUseNavSystem);
    virtual bool GetUseNavSystem();

    NiNavOrbitControllerPtr m_spOrbitController;
    NiNavFlyControllerPtr m_spFlyController;

    NiUIMemberSlot1<NiSample, unsigned char> m_kHideAll;
    NiUIMemberSlot0<NiSample> m_kToggleNavHelp;
    NiUIMemberSlot0<NiSample> m_kChangeController;
    NiUIMemberSlot0<NiSample> m_kQuit;

    void ToggleNavHelp();
    virtual void ChangeController();
    void HideAllPressed(unsigned char ucState);
    virtual void UpdateControllerLog(NiNavBaseController* pkNewController);

    // Layout controls
    unsigned int m_uiMaxOutputLogLines;
    float m_fUIElementHeight;
    float m_fUIElementWidth;
    float m_fUIGroupHeaderHeight;
    NiPoint2 m_kUINavHelpStart;
    NiPoint2 m_kUIElementGroupOffset;

    void AddDefaultUIElements(NiUIGroup* pkGroup, float& fLeftOffset,
        float& fTopOffset, float fElementWidth, float fElementHeight);
    void AddNavigationHelpUIElements(float fLeftOffset, float fTopOffset, 
        float fElementWidth, float fElementHeight, float fImageWidth,
        float fMinLabelWidth);

    unsigned int AddLogEntry(const char* pcNewLine);
    bool ChangeLogEntry(unsigned int uiLineNum, const char* pcNewLine);

    // Log elements
    void ReinitializeLogText();
    NiUILabelPtr m_spOutputLog;
    NiTPrimitiveArray<NiString*> m_kLogLines;
    unsigned int m_uiNumLogChars;
    unsigned int m_uiNavControllerLine;
    NiUILabelPtr m_spFrameRateLabel;
    float m_fLastFrameRateTime;

    // Navigation Help elements
    bool m_bShowNavHelp;
    NiTObjectArray<NiUIRenderGroupPtr> m_kNavHelpRenderGroups;

#if defined(WIN32) || defined(_WII)
    NiCursorPtr m_spCursor;
    NiCursorRenderClickPtr m_spCursorRenderClick;
#endif

#if defined(WIN32)
    virtual bool OnDefault(NiEventRef pEventRecord);
    static unsigned int NiSampleMessageBoxFunc(const char* pcText, 
        const char* pcCaption, void* pvExtraData = NULL);
    bool FindSampleDataFile(const char* pcFilename, char* pcFullPath);
#endif

#if defined(_WII)
    NiTObjectArray<NiMaterialPtr> m_kWiiMaterialArray;
    NiTexturePtr m_spMenuPointer;
    NiTexturePtr m_spNavPointer;
#endif

    NiPoint3 m_kNavUpAxis;
    float m_fNavDefaultScale;

#if defined(_USRDLL)
    NiTObjectArray<NiString> m_kShaderLibraries;
    NiTObjectArray<NiString> m_kParserLibraries;
#else
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_CLASSCREATIONCALLBACK> 
        m_kShaderLibraries;
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_RUNPARSERCALLBACK>
        m_kParserLibraries;
#endif

    unsigned int m_uiActiveCallbackIdx;

    // User interface pathing
    char m_acSkinPath[NI_MAX_PATH];

    // Get/set the path to the UI skin file.  The value set here will have
    // no effect after NiSample::Initialize() is called, as it is only used
    // in CreateUIElements()
    const char* GetUISkinFilename() const;
    void SetUISkinFilename(const char* pcFilename);

    // Frame rendering system object names.
    NiFixedString m_kUIManagerRenderStepName;
    NiFixedString m_kUIManagerRenderClickName;
    NiFixedString m_kCursorRenderStepName;
    NiFixedString m_kCursorRenderClickName;

    bool m_bUseNavSystem;
};

#include "NiSample.inl"

#endif
