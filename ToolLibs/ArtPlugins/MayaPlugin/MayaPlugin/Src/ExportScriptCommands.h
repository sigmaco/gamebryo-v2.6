// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef EXPORTSCRIPTCOMMANDS_H
#define EXPORTSCRIPTCOMMANDS_H

#include "maya/MPxCommand.h"
//---------------------------------------------------------------------------
class OpenSelectScriptWindow : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class GetGamebryoRunSilently : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class SetGamebryoRunSilently : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class GetGamebryoUseLastSavedScript : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class SetGamebryoUseLastSavedScript : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class GetGamebryoForceDefaultScript : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
class SetGamebryoForceDefaultScript : public MPxCommand 
{ 
public:
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
};
//---------------------------------------------------------------------------
#endif
