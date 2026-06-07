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

#ifndef NIEXPORTPLUGININFO_H
#define NIEXPORTPLUGININFO_H

#include "NiPluginInfo.h"

/// The container for all script-relevant information about a specific plugin.
class NIPLUGINTOOLKIT_ENTRY NiExportPluginInfo : public NiPluginInfo
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiExportPluginInfo();

    /// The constructor takes the directory, file name, and extension.
    NiExportPluginInfo(const char* pcDirectory, const char* pcFile,
        const char* pcExtension);

    /// Virtual destructor.
    inline virtual ~NiExportPluginInfo(){}

    /// Make a deep copy of the the plugin info
    virtual NiPluginInfo* Clone(NiPluginInfo* pkClone);

    /// Does the input script equal the current plugin
    virtual bool Equals(NiPluginInfo* pkInfo);

    /// Get the directory for this object.
    const NiString& GetDirectory() const;

    /// Get the file name for this object.
    const NiString& GetFile() const;

    /// Get the file extension for this object.
    const NiString& GetExtension() const;

    /// Set the directory for this object.
    void SetDirectory(const char* pcDirectory);

    /// Set the file name for this object.
    void SetFile(const char* pcFile);

    /// Set the file extension for this object.
    void SetExtension(const char* pcExtension);

    /// Get the full, contatenated file path.
    NiString GetFullFilePath() const;

protected:
    /// The directory for this object.
    NiString m_strDirectory;

    /// The file name for this object.
    NiString m_strFile;

    /// The file extension for this object.
    NiString m_strExtension;
};

NiSmartPointer(NiExportPluginInfo);

#endif  // #ifndef NIEXPORTPLUGININFO_H
