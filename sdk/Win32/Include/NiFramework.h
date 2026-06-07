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

#ifndef NIFRAMEWORK_H
#define NIFRAMEWORK_H

#include "NiPluginToolkitLibType.h"
#include <NiRTTI.h>
#include "NiString.h"
#include "NiBatchExecutionResult.h"

class NiSharedDataList;
class NiScriptInfo;
class NiExecutionContext;

/// Base framework class for NiPluginToolkit. This class is not GUI-specific.
class NIPLUGINTOOLKIT_ENTRY NiFramework : public NiMemObject
{
public:
    NiDeclareRootRTTI(NiFramework);

    /// Retrieves the framework.
    /// @return A reference to the framework.
    static NiFramework& GetFramework();

    /// Creates the framework. Should be called by the application once
    /// during main execution.
    /// @param pcAppPath The path to the application's executable file.
    /// @param pcAppName The name of the application.
    /// @param pcAppVersion The version number of the application.
    /// @param bFloodgateParallelExecution An optional flag to set floodgate 
    /// in parallel mode.
    static void InitFramework(const char* pcAppPath, const char* pcAppName,
        const char* pcAppVersion, bool bFloodgateParallelExecution = true);

    /// Destroys the framework. Should be called by the application once
    /// during main execution, after all GUI and other components have been
    /// destroyed.
    static void ShutdownFramework();

    /// Gets the absolute path to the application directory.
    /// @return The path to the application's executable file.
    const NiString& GetAppPath();

    /// Gets the application's name. Used for plugin compatibility.
    /// @return The name of the application.
    const NiString& GetAppName();

    /// Gets the application's version number. Used for plugin
    /// compatibility.
    /// @return The version number of the application.
    const NiString& GetAppVersion();

    /// Gets the Gamebryo version the application framework is built
    /// with.
    /// @return The Gamebryo version number for this framework.
    const char* GetNiVersion();

    /// Retrieves the shared data list.
    /// @return A reference to the shared data list.
    NiSharedDataList& GetSharedDataList() const;

    /// Executes plug-ins using the specified script and execution context.
    /// This function simply calls the plug-in manager's ExecuteScript
    /// function.
    /// @param pkScript An NiScriptInfo object corresponding to the script
    ///                 to execute.
    /// @return A smart pointer to the batch execution result.
    NiBatchExecutionResultPtr ExecuteScript(NiScriptInfo* pkScript);


protected:
    /// Protected default constructor. This is protected to enforce the
    /// Singleton pattern.
    NiFramework(){}

    /// Protected constructor. This is protected to enforce the Singleton
    /// pattern.
    NiFramework(const char* pcAppPath, const char* pcAppName,
        const char* pcAppVersion);

    /// Protected copy constructor. This is protected to enforce the
    /// Singleton pattern.
    NiFramework(NiFramework&){}

    /// Protected virtual destructor. This is protected to enforce the
    /// Singleton pattern.
    inline virtual ~NiFramework(){}

    /// A pointer to the one and only NiFramework instance.
    static NiFramework* ms_pkThis;

    /// The path to the application's executable file.
    NiString m_strAppPath;

    /// The name of the application.
    NiString m_strAppName;

    /// The version number of the application.
    NiString m_strAppVersion;
};

#endif  // #ifndef NIFRAMEWORK_H
