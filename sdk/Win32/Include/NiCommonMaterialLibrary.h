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

#ifndef NICOMMONMATERIALLIBRARY_H
#define NICOMMONMATERIALLIBRARY_H

#include "NiCommonMaterialLibType.h"

#include <NiMaterialLibrary.h>
#include <NiShaderDesc.h>

NiSmartPointer(NiShaderLibraryDesc);

class NICOMMONMATERIAL_ENTRY NiCommonMaterialLibrary : 
    public NiMaterialLibrary
{
protected:
    NiCommonMaterialLibrary();
    virtual ~NiCommonMaterialLibrary();

public:
    virtual NiMaterial* GetMaterial(const NiFixedString& kName);
    virtual const char* GetMaterialName(unsigned int ui);
    virtual unsigned int GetMaterialCount() const;

    virtual NiShaderDesc* GetFirstMaterialDesc();
    virtual NiShaderDesc* GetNextMaterialDesc();

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    static NiCommonMaterialLibrary* Create();
    // *** end Emergent internal use only ***

    enum 
    {
        NUM_COMMON_MATERIALS = 3
    };

protected:
    static NiCommonMaterialLibrary* ms_pkThis;

    NiShaderDesc* m_apkShaderDesc[NUM_COMMON_MATERIALS];
    static unsigned int ms_uiMaterialDesc;
};

NiSmartPointer(NiCommonMaterialLibrary);

#endif  //NICOMMONMATERIALLIBRARY_H
