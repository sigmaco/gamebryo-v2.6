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
#include "GamebryoExporterHelpers.h"

#include "CrosswalkGamebryoHelpers.h"

#include <xsi_context.h>
#include <xsi_property.h>
#include <xsi_command.h>
#include <xsi_argument.h>
#include <xsi_application.h>
#include <xsi_model.h>

using namespace XSI; 

namespace
{
    //---------------------------------------------------------------------------
    // Define the XSI command.
    //
    // We always use teh same command argument name for most commands,
    // for simplicity and consistency.
    //---------------------------------------------------------------------------
    CStatus CreateGamebryoCommand(const CRef& in_context)
    {
        CStatus status = CStatus::OK;
        Context ctx(in_context);
        Command cmd(ctx.GetSource());

        cmd.EnableReturnValue(true) ; 

        ArgumentArray args = cmd.GetArguments();
        args.Add(L"ObjectNameToCreateIn", L"\0");

        return status;
    }

    //---------------------------------------------------------------------------
    // Create a specifed property on a specified object.
    //---------------------------------------------------------------------------
    static CStatus CreateGamebryoProperty(const CRef& in_context, const CString& in_PropName)
    {
        Context ctx(in_context);

        // Access the arguments to the command
        CValueArray		args = ctx.GetAttribute(L"Arguments");
        CString			argObjectName(args[0]);

        // Find the object specified in argument.
        Application app;
        Model sceneRoot = app.GetActiveSceneRoot();
        X3DObject object;
        if (!argObjectName.IsEmpty())
            object = sceneRoot.FindChild(argObjectName, CString(), CStringArray());
        else
            object = sceneRoot;
        if (!object.IsValid())
            return CStatus::Fail;

        // Create the property if it doesn't already exist.
        Property l_Property = object.GetProperties().GetItem(in_PropName);
        if (!l_Property.IsValid())
            l_Property = object.AddProperty(in_PropName, false, in_PropName);

        // Set return value.
        CValue l_Value(l_Property);
        ctx.PutAttribute(L"ReturnValue", l_Value);

        return CStatus::OK;
    }

    //---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
// Gamebryo-export custom properties creation command.
// This is provided solely for the benefits of script authors, so that
// the properties can be created without showing a dialog.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateExportGamebryoOptions_Init(const CRef& in_context)
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);
    Command cmd(ctx.GetSource());

    cmd.EnableReturnValue(true) ; 

    ArgumentArray args = cmd.GetArguments();
    args.Add(L"ObjectNameToCreateIn", L"\0");
    args.Add(L"PropertyNameToBeCreated", XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W L"\0");

    return status;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateExportGamebryoOptions_Execute(CRef& in_context)
{
    Context ctx(in_context);

    // Access the arguments to the command
    CValueArray		args = ctx.GetAttribute(L"Arguments");
    CString			argObjectName(args[0]);
    CString			argOptionName(args[1]);

    // Create the property if it doesn't already exist.
    Property l_Property;
    bool result = GetExportOptionProperty(l_Property, argObjectName, argOptionName);

    // Set return value.
    CValue l_Value(l_Property);
    ctx.PutAttribute(L"ReturnValue", l_Value);

    return result ? CStatus::OK : CStatus::Fail;
}

//---------------------------------------------------------------------------
// Create the Gamebryo Z-Buffer property on a given object.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoZBuffer_Init(const CRef& in_context)
{
    return CreateGamebryoCommand(in_context);
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoZBuffer_Execute(CRef& in_context)
{
    return CreateGamebryoProperty(in_context, XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME_W);
}

//---------------------------------------------------------------------------
// Create the Gamebryo level-of-details property on a given object.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoLOD_Init(const CRef& in_context)
{
    return CreateGamebryoCommand(in_context);
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoLOD_Execute(CRef& in_context)
{
    return CreateGamebryoProperty(in_context, XSI_GAMEBRYO_LOD_PROPERTY_NAME_W);
}

//---------------------------------------------------------------------------
// Create the Gamebryo lighting property on a given object.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoLighting_Init(const CRef& in_context)
{
    return CreateGamebryoCommand(in_context);
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoLighting_Execute(CRef& in_context)
{
    return CreateGamebryoProperty(in_context, XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME_W);
}

//---------------------------------------------------------------------------
// Create the Gamebryo shadow property on a given object.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoShadow_Init(const CRef& in_context)
{
    return CreateGamebryoCommand(in_context);
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus CreateGamebryoShadow_Execute(CRef& in_context)
{
    return CreateGamebryoProperty(in_context, XSI_GAMEBRYO_SHADOW_PROPERTY_NAME_W);
}

//---------------------------------------------------------------------------
