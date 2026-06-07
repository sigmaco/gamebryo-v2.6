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

#ifndef NIPLUGIN_H
#define NIPLUGIN_H

#include "NiPluginToolkitLibType.h"
#include <NiRefObject.h>
#include "NiPluginToolkitDefinitions.h"
#include <NiRTTI.h>
#include "NiString.h"
#include "NiExecutionResult.h"
#include "NiSharedData.h"
class NiSharedDataList;
class NiPluginInfo;

/// Base class for all plug-ins.
class NIPLUGINTOOLKIT_ENTRY NiPlugin : public NiRefObject
{
public:
    NiDeclareRootRTTI(NiPlugin);

    /// The constructor takes strings for the name, version, and
    /// descriptions.
    NiPlugin(const char* pcName, const char* pcVersion,
        const char* pcShortDescription, const char* pcLongDescription);

    /// Virtual destructor.
    virtual ~NiPlugin();

    /// Gets the plug-in name.
    /// @return The name of this particular plug-in.
    const NiString& GetName() const;

    /// Gets the plug-in version.
    /// @return The version of this particular plug-in.
    const NiString& GetVersion() const;

    /// Get the short description of this plug-in.
    /// @return The short description of this particular plug-in.
    const NiString& GetShortDescription() const;
    
    /// Get the long description of this plug-in.
    /// @return The long description of this particular plug-in.
    const NiString& GetLongDescription() const;

    /// Set the module handle for this plug-in.
    /// @param hModule The handle of the module.
    void SetModule(NiModuleRef hModule);

    /// Get the module handle for this plug-in.
    /// @return The handle of the module.
    NiModuleRef GetModule() const;

    /// Determines whether or not this plug-in can handle this plugin info
    /// object. This is useful for doing versioning, as it allows plug-ins
    /// to take responsiblity for executing an NiPluginInfo object
    /// arbitrarily.
    /// @return Whether or not this plug-in can handle the NiPluginInfo
    ///         struct.
    virtual bool CanExecute(NiPluginInfo* pkInfo) = 0;

    /// Execute the NiPluginInfo script using the specified execution context.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo) = 0;

    /// Get the default parameters for this plugin
    virtual NiPluginInfo* GetDefaultPluginInfo() = 0; 

    /// Does this plugin have the ability to bring up a dialog box to
    /// edit its parameters?
    virtual bool HasManagementDialog() = 0;

    /// Create a management dialog using the input as initial values.
    /// @param pkInitialInfo The initial set of plug-in preferences.
    /// @return whether or not the info changed.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent) = 0;

    /// Set the path where this plugin can be found
    virtual void SetAbsolutePluginPath(const char* pcPath);

    /// Get the path where this plugin can be found
    virtual const NiString& GetAbsolutePluginPath();

    /// Determine whether or not this plugin can instantiate 
    /// a default version of its options.
    virtual bool CanCreateDefaultInstance();

protected:
    /// A helper function that verifies that the version string passed in
    /// to the function is less than or equal to the actual version of this
    /// plug-in.
    bool VerifyVersionString(const char* pcVersionString);

    // Helper functions for comparing version strings. Each function will
    // return true if the first operand is less than the second.
    bool CompareVersions(const char* pcA, const char* pcB);
    bool CompareVersions(unsigned int uiA, const char* pcB);
    bool CompareVersions(const char* pcA, unsigned int uiB);

    /// Get Shared data list
    static NiSharedDataList* GetSharedDataList();

    /// Get Shared data by class name
    static NiSharedData* GetSharedData(NiSharedDataType pkType);

    /// The plug-in name. 
    NiString m_strName;

    /// The plug-in version. 
    NiString m_strVersion;

    /// The description of this plugin
    NiString m_strShortDescription;

    /// The description of this plugin
    NiString m_strLongDescription;

    /// The path to this plugin
    NiString m_strAbsolutePluginPath;

    /// The DLL module for this plug-in.
    NiModuleRef m_hModule;
};

NiSmartPointer(NiPlugin);

#endif  // #ifndef NIPLUGIN_H
