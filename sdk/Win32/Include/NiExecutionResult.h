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

#ifndef NIEXECUTIONRESULT_H
#define NIEXECUTIONRESULT_H

#include "NiPluginToolkitLibType.h"
#include <NiRefObject.h>
#include <NiRTTI.h>
#include <NiSmartPointer.h>

/// The containter for the results of a plugin execution.
class NIPLUGINTOOLKIT_ENTRY NiExecutionResult : public NiRefObject
{
public:
    NiDeclareRootRTTI(NiExecutionResult);

    /// The possible results of the script execution. 
    enum ReturnCode 
    {
        EXECUTE_SUCCESS,
        EXECUTE_FAILURE
    };

    /// The constructor accepts the return code.
    NiExecutionResult(ReturnCode eReturnCode);
    
    /// The exact result of the script execution. 
    ReturnCode m_eReturnCode;
};

NiSmartPointer(NiExecutionResult);

#endif  // #ifndef NIEXECUTIONRESULT_H
