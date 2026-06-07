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

#ifndef NIBATCHEXECUTIONRESULT_H
#define NIBATCHEXECUTIONRESULT_H

#include "NiPluginToolkitLibType.h"
#include "NiExecutionResult.h"
#include <NiTArray.h>

class NiScriptInfo;

/// The containter for the results of a script execution.
class NIPLUGINTOOLKIT_ENTRY NiBatchExecutionResult : public NiExecutionResult
{
public:
    NiDeclareRTTI;

    /// The constructor accepts the return code.
    NiBatchExecutionResult(ReturnCode eReturnCode);

    /// An array containing the plugin results.
    NiTObjectArray<NiExecutionResultPtr> m_pkPluginResults;

    bool WasSuccessful();
};

NiSmartPointer(NiBatchExecutionResult);

#endif  // #ifndef NIBATCHEXECUTIONRESULT_H
