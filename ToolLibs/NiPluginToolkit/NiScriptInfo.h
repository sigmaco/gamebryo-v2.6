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

#ifndef NISCRIPTINFO_H
#define NISCRIPTINFO_H

#include "NiPluginToolkitLibType.h"
#include <NiRefObject.h>
#include "NiPluginToolkitDefinitions.h"
#include <NiSmartPointer.h>
#include <NiTPtrSet.h>
#include "NiPluginInfo.h"

#define SCRIPT_HEADER "Version:"
#define SCRIPT_VERSION "1"

/// The container of all script-relevant information about which plugins
/// should be executed.

typedef NiTPtrSet<NiPluginInfoPtr, NiTNewInterface<NiPluginInfoPtr> >     
    NiPluginInfoPtrSet;

class NIPLUGINTOOLKIT_ENTRY NiScriptInfo : public NiRefObject
{
public:
    /// Default constructor
    NiScriptInfo();

    /// Gets the name of this particular script.
    const NiString& GetName() const;
    
    /// Sets the name of this particular script.
    void SetName(const char* pcName);

    /// Gets the type of this particular script
    const NiString& GetType() const;

    /// Sets the type of this particular script
    void SetType(const char* pcType);
            
    /// Gets the list of plugins used by this script in the correct order.
    NiPluginInfoPtrSet& GetPluginSet();

    /// Add plugin to the list
    void AddPluginInfo(NiPluginInfo* pkInfo);

    /// Remove a plugin from the list
    void RemovePlugin(NiPluginInfo* pkInfo);

    /// Make a deep copy of the the script info and return it in a new info
    /// structure.
    NiScriptInfo* Clone();

    /// Make a deep copy of pkScript into this (equivalent to an assignment
    /// operator for this class).
    void CopyMembers(NiScriptInfo* pkScript);

    /// Does the input script equal the current script
    bool Equals(NiScriptInfo* pkScript);

    /// Mark this script as needing to be saved or not
    void MarkAsDirty(bool bMark);

    /// Does this script need to be saved?
    bool IsDirty();

    /// Set the file associated with this script
    void SetFile(const char* pcFileName);

    /// Get the file assoicated with this script
    const NiString& GetFile();

    /// Get plugin at index uiIdx
    NiPluginInfo* GetPluginInfoAt(unsigned int uiIdx);

    /// Get the total number of plugins
    unsigned int GetPluginInfoCount();

    /// Get PluginInfo index
    unsigned int GetPluginInfoIndex(NiPluginInfo* pkPlugin);

    /// Move plugin to position
    void MovePluginInfoToPosition(NiPluginInfo* pkPlugin, unsigned int uiIdx);

    /// Remove the plugininfo at position uiIdx
    NiPluginInfoPtr RemovePluginInfoAt(unsigned int uiIdx);

    /// Make the values of this Script info match the input script info
    void ForceMatch(NiScriptInfo* pkInfo);

protected:
    /// The name of this script.
    NiString m_strName;
    
    /// The application-defined type of this script
    NiString m_strType;

    /// The list of plugins and their parameters used by this script.
    NiPluginInfoPtrSet m_kPluginSet;

    /// Determines if this script needs to be saved
    bool m_bDirty;

    /// The filename that is associated with this script
    NiString m_strFile;
};


NiSmartPointer(NiScriptInfo);

#endif  // #ifndef NISCRIPTINFO_H
