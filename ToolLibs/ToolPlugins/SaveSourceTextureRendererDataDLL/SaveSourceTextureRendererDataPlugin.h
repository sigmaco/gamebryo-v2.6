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


#ifndef SAVESOURCETEXTURERENDERERDATAPLUGIN_H
#define SAVESOURCETEXTURERENDERERDATAPLUGIN_H

#include <NiTPointerList.h>

#include <NiPersistentSrcTextureRendererData.h>

class NiAVObject;
class NiPixelData;
class NiTexture;
class NiTimeController;

// This plug-in is Gamebryo's Save Source Texture Renderer Data process
// plug-in.  During processing, it streams in source texture pixel data and
// creates streamable persistent source texture renderer data, which is
// subsequently streamed out for the final Nif file during export.  In this
// way, platform-specific source texture renderer data may be loaded directly
// into the appropriate platform at application load time, without memory
// allocations for conversion.  The goal of this plug-in is to perform
// platform-specific conversions at export time, thereby improving "high water
// mark" behavior for texture memory at load time on specific platforms.  (See
// also the description of NiPersistentSourceTextureRendererData in the
// Gamebryo documentation for more details.)
class SaveSourceTextureRendererDataPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;  // RTTI declaratioin macro.

    SaveSourceTextureRendererDataPlugin();  // Default constructor.
    ~SaveSourceTextureRendererDataPlugin(); // Destructor.

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);

    void LoadImageTilerLibrary();

protected:
    void CreatePersistentSrcRendererData(NiSourceTexture* pkTexture);

    void ProcessFlipController(NiTimeController* pkControl);
    void ProcessTexture(NiTexture* pkTexture);

    void Traverse(NiAVObject* pkScene);

    bool m_bWriteResultsToLog;
    NiXMLLogger* m_pkLogger;

    typedef bool (*NiTileImageFunction)
        (NiPersistentSrcTextureRendererData*);

    HMODULE  m_hImageTilerDLL[NiPersistentSrcTextureRendererData::
        NI_NUM_PLATFORM_IDS];
    NiTileImageFunction 
        m_pfnTileImageFunc[NiPersistentSrcTextureRendererData::
        NI_NUM_PLATFORM_IDS];

    static const char* const
        ms_pcImageTilerDLLName[NiPersistentSrcTextureRendererData::
        NI_NUM_PLATFORM_IDS];
};

#endif  // #ifndef SAVESOURCETEXTURERENDERERDATAPLUGIN_H
