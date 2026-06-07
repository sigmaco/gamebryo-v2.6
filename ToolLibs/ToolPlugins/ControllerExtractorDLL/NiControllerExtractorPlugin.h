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

#ifndef NICONTROLLEREXTRACTORPLUGIN_H
#define NICONTROLLEREXTRACTORPLUGIN_H

class NiControllerExtractorPlugin : public NiExportPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiControllerExtractorPlugin();

    /// Required virtual function overrides from NiExportPlugin.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this export plug-in
    /// info object. This function should be overridden by export plug-ins.
    /// @return Whether or not this plug-in can handle the NiExportPluginInfo
    ///         object.
    virtual bool CanExport(NiExportPluginInfo* pkInfo);

    /// Execute the NiExportPluginInfo script.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr Export(const NiExportPluginInfo* pkInfo);

    /// Gets the string describing the supported file types.
    virtual const char* GetFileDescriptorString();

    /// Gets the string with the supported file type extensions.
    virtual const char* GetFileTypeString();

protected:
    NiProgressDialog* m_pkProgressBar;

    void GetGlobalOptionsForCharRoot(
        const NiControllerExtractor::NiCCAGlobalInfoArray& kArray,
        NiAVObject* pkCharRoot, bool& bCompress, bool& bCompactCPs,
        float& fKeyPercentage);
    bool GetOverrideOptionsForSequence(
        const NiControllerExtractor::NiCCAInfoArray& kArray,
        const NiSequenceData* pkSeqData, 
        const NiControllerExtractor::SequenceInfo* pkSeqInfo, 
        bool& bCompress, bool& bCompactCPs, float& fKeyPercentage);
    void TransferOverrideOptionsToNonAccum(
        NiControllerExtractor::NiCCAInfoArray& kArray,
        NiAVObject* pkAccumRoot);
    void SetObjectOverrideOptionsForSequence(
        const NiControllerExtractor::NiCCAInfoArray& kArray,
        const NiSequenceData* pkSeqData, 
        const NiControllerExtractor::SequenceInfo* pkSeqInfo);
    void RemoveObjectOverrideOptionsForSequence(
        const NiControllerExtractor::NiCCAInfoArray& kArray,
        const NiSequenceData* pkSeqData, 
        const NiControllerExtractor::SequenceInfo* pkSeqInfo);
};

NiSmartPointer(NiControllerExtractorPlugin);

#endif  // #ifndef NICONTROLLEREXTRACTORPLUGIN_H
