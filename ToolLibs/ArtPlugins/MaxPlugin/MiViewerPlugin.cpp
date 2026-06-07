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

#include "MaxImmerse.h"
#include "MiViewerPlugin.h"
#include "MiUtils.h"

//---------------------------------------------------------------------------

IParamMap* MiViewerPlugin::m_pAboutPMap = NULL;
#ifdef QUICKSAVE
IParamMap* MiViewerPlugin::m_pQuickSavePMap = NULL;
#endif
IParamMap* MiViewerPlugin::m_pRendererPMap = NULL;
IParamMap* MiViewerPlugin::m_pInteractionPMap = NULL;
IParamMap* MiViewerPlugin::m_pViewerLoaderPMap = NULL;

//---------------------------------------------------------------------------
static MiViewerPlugin NvViewerPlugin;
//---------------------------------------------------------------------------

class MiViewerPluginClassDesc : public ClassDesc 
{
public:
    inline int             IsPublic() { return 1; }
    inline void*           Create(BOOL) { return &NvViewerPlugin; }
    inline const TCHAR*    ClassName() { return GetString(IDS_NI_UTIL_CLASS_NAME); }
    inline SClass_ID       SuperClassID() {return UTILITY_CLASS_ID; }
    inline Class_ID        ClassID() { return MI_UTIL_CLASS_ID; }
    inline const TCHAR*    Category() {return _T("");}
};
//---------------------------------------------------------------------------
static MiViewerPluginClassDesc NvViewerPluginDesc;

//---------------------------------------------------------------------------
ClassDesc* GetMAXViewerClassDesc() 
{
    return &NvViewerPluginDesc;
}

//---------------------------------------------------------------------------
MiViewerPlugin::MiViewerPlugin()
{ 

    m_pIntf = NULL; 
    m_iFlags = DEFAULT_ROLLUP; 
}

//---------------------------------------------------------------------------
MiViewerPlugin::~MiViewerPlugin()
{
}
//---------------------------------------------------------------------------

