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

#ifndef NIMATERIALHELPERS_H
#define NIMATERIALHELPERS_H

#include "NiMaterialToolkitLibType.h"

#include <NiAVObject.h>
#include <NiTList.h>

class NIMATERIALTOOLKIT_ENTRY NiMaterialHelpers
{
public:
    static bool RegisterMaterials(NiAVObject* pkScene, 
        NiRenderer* pkRenderer);
    static bool UnRegisterMaterials(NiAVObject* pkScene, 
        NiRenderer* pkRenderer, bool bReplace = false);
    
protected:
    static void RecursivelyRegisterMaterialsHelper(NiRenderer* pkRenderer, 
        NiAVObject* pkObj, NiTList<const char*>& kErrList);
    static void RecursivelyReleaseMaterialsHelper(NiRenderer* pkRenderer, 
        NiAVObject* pkObj, bool bReplace);
};
#endif