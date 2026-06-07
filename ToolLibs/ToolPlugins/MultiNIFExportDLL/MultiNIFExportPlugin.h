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

#ifndef MULTINIFEXPORTPLUGIN_H
#define MULTINIFEXPORTPLUGIN_H

/// This is our MultiNIF export plug-in. It currently has no functionality
/// and is meant to be a template from which customers can create their own
/// export plug-ins.
class MultiNIFExportPlugin : public NiExportPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    MultiNIFExportPlugin();

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

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiExportPlugin.
    //-----------------------------------------------------------------------
    /// Determines whether or not this plug-in can handle this export plug-in
    /// info object. This function should be overridden by export plug-ins.
    virtual bool CanExport(NiExportPluginInfo* pkInfo);

    /// Execute the NiExportPluginInfo script.
    virtual NiExecutionResultPtr Export(const NiExportPluginInfo* pkInfo);

    /// Gets the string describing the supported file types.
    virtual const char* GetFileDescriptorString();

    /// Gets the string with the supported file type extensions.
    virtual const char* GetFileTypeString();

    virtual bool IsPrimaryExportPlugin();


protected:

    void RecursiveFindExportNodes(NiAVObject* pkObject);

    bool IsExportNode(NiAVObject* pkObject, NiString& kFilename);

    bool ValidExportNode(NiAVObject* pkObject, NiAVObject* pkExportNode,
        bool bDescendentOfExportNode);

    void ExportNode(NiAVObject* pkObject, NiString& kFilename);

    bool CheckForSkinAndBones(NiAVObject* pkObject, NiAVObject* pkRoot,
        bool bDescendentOfRoot);

    bool CheckForParticleSystemDependencies(NiAVObject* pkObject, 
        NiAVObject* pkRoot, bool bMustDepend);

    bool CheckForPortalDependencies(NiAVObject* pkObject, NiAVObject* pkRoot);

    bool IsValidFilename(const char* pcFilename);

    bool IsDescendentOf(NiAVObject* pkObject, NiAVObject* pkRoot);

    void DuplicateSceneData(NiAVObject* pkNewRoot);

    void ShowDependencyError(const char* pcRootName,
        const char* pcObjectType, const char* pcObjectName, 
        const char* pcDependencyType, const char* pcDependentObject);

    void InsertCameras(NiAVObject* pkObj, NiStream* pkStream);

    NiString m_kDirectory;
    NiString m_kFile;
    bool m_bGlobalUseObjectNameAsFileName;
    bool m_bGlobalDeleteAfterExport;
    int m_iGlobalTranslateType;
    int m_iGlobalRotateType;
    int m_iGlobalScaleType;
    bool m_bGlobalExportLittleEndian;

    bool m_bLocalUseObjectNameAsFileName;
    bool m_bLocalDeleteAfterExport;
    int m_iLocalTranslateType;
    int m_iLocalRotateType;
    int m_iLocalScaleType;

    NiAVObject* m_pkCurrentSceneRoot;
};

#endif  // #ifndef MULTINIFEXPORTPLUGIN_H

