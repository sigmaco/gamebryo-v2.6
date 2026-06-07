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

#ifndef MESHFINALIZEPLUGIN_H
#define MESHFINALIZEPLUGIN_H

#include "NiPluginToolkitDefinitions.h"
#include <NiMeshProfileErrorHandler.h>

class NiMeshProfileProcessor;

class MeshFinalizePlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    MeshFinalizePlugin();

    // Destructor.
    ~MeshFinalizePlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

protected:
    unsigned int CountMeshes(NiAVObject* pkObject);
    void ReportErrors();
    static void ErrorPrintf(const char* pcFormat, ...);

    static void ProgressInitCB(unsigned int uiSize);
    static void ProgressCB(NiMeshProfileErrorHandler& kMessageHandler);
    static void RendererFinalizeProgressCB(NiUInt32 uiTotalMeshCount, NiUInt32 uiCurrentMesh);

    const char* GetRendererFinalizeLibraryName();
    void LoadRendererFinalizeLibraries();
    void UnloadRendererFinalizeLibraries();

    NiProgressDialog* m_pkProgressBar;
    NiString m_strMessage;
    bool m_bErrorsOrWarningsFound;
    static MeshFinalizePlugin* ms_pkThis;


    struct RendererFinalizeDLLInfo
    {
        NiSystemDesc::RendererID m_eRendererID;
        const char* m_pcDLLName;
    };

    // The signature of this function must remain consistent with the NiMeshRendererFinalizeFunc
    // typedef in NiWiiMeshFinalize. Any changes must be reflected in both places.
    typedef bool (*NiMeshRendererFinalizeFunc)(NiTLargeObjectArray<NiObjectPtr>& kTopObjects, 
        NiTFixedStringMap<const char*>& kOptions, void (*pfnProgressCB)(NiUInt32, NiUInt32), 
        void (*pfnErrorPrintf)(const char*, ...));

    static RendererFinalizeDLLInfo ms_kRendererFinalizeDLLInfo[];

    NiTPointerMap<NiSystemDesc::RendererID, HMODULE> m_kFinalizeDLLMap;
    NiTPointerMap<NiSystemDesc::RendererID, NiMeshRendererFinalizeFunc> m_kPreFinalizeFuncMap;
    NiTPointerMap<NiSystemDesc::RendererID, NiMeshRendererFinalizeFunc> m_kPostFinalizeFuncMap;
};

#endif  // #ifndef MESHFINALIZEPLUGIN_H
