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

#ifndef NISINGLESHADERMATERIALLIBRARY_H
#define NISINGLESHADERMATERIALLIBRARY_H

#include "NiMaterialLibrary.h"
class NiShaderLibrary;

// NiSingleShaderMaterialLibrary
class NIMAIN_ENTRY NiSingleShaderMaterialLibrary : public NiMaterialLibrary
{
public:
    virtual ~NiSingleShaderMaterialLibrary();

    virtual NiMaterial* GetMaterial(const NiFixedString& kName);

    virtual NiShaderDesc* GetFirstMaterialDesc();
    virtual NiShaderDesc* GetNextMaterialDesc();
    virtual unsigned int GetMaterialCount() const;
    virtual const char* GetMaterialName(unsigned int uiMaterial);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiSingleShaderMaterialLibrary();
    static NiSingleShaderMaterialLibrary* ms_pkThis;
    NiShaderLibrary* m_pkLastLibrary;
    unsigned int m_uiLastDesc;
};

typedef NiPointer<NiSingleShaderMaterialLibrary> 
    NiSingleShaderMaterialLibraryPtr;

#endif  //#ifndef NIMATERIALLIBRARY_H
