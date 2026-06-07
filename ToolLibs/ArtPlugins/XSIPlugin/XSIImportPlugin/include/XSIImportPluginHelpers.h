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

#ifndef XSI_IMPORT_PLUGIN_HELPERS_H
#define XSI_IMPORT_PLUGIN_HELPERS_H

#include "NiPluginToolkitLibType.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class NIPLUGINTOOLKIT_ENTRY NiXMLLogger;
class NIPLUGINTOOLKIT_ENTRY NiSharedDataList;

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI import plugin helper functions.
    //---------------------------------------------------------------------------

    /*! Try to find the XML logger in the shared data.
    */
    NiXMLLogger * FindXMLLogger(NiSharedDataList* pkDataList);
}

#endif /* XSI_IMPORT_PLUGIN_HELPERS_H */
