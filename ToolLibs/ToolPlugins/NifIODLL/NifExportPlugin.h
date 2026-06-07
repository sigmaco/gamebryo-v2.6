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

#ifndef NIFEXPORTPLUGIN_H
#define NIFEXPORTPLUGIN_H

class NifExportPlugin : public NiExportPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NifExportPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    // Required virtual function overrides from NiExportPlugin.
    virtual bool CanExport(NiExportPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Export(const NiExportPluginInfo* pkInfo);
    virtual const char* GetFileDescriptorString();
    virtual const char* GetFileTypeString();

    void InsertCameras(NiAVObject* pkObj, NiStream* pkStream);
};

#endif  // #ifndef NIFEXPORTPLUGIN_H
