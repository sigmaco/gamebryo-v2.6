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

#include "GamebryoExporter.h"
#include "GamebryoExporterStrings.h"

#include <xsi_context.h>
#include <xsi_command.h>

using namespace XSI; 

//---------------------------------------------------------------------------
// Gamebryo version command. reports version info to user.
// This is provided solely for the benefits of script authors, so that
// the version of the plugin, Corsswalk and Gamebryo can be queried.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK CStatus GamebryoVersion_Init( const CRef& in_context )
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);
    Command cmd(ctx.GetSource());

    cmd.EnableReturnValue( true ) ; 

    return status;
}
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoVersion_Execute( CRef& in_context )
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);

    CValue l_Value(XSI_GAMEBRYO_EXPORT_PLUGIN_API_VERSIONS_STRING);
    ctx.PutAttribute( L"ReturnValue", l_Value );

    return status;
}
//---------------------------------------------------------------------------
