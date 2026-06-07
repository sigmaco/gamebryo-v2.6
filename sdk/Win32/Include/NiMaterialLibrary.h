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

#ifndef NIMATERIALLIBRARY_H
#define NIMATERIALLIBRARY_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiShaderDesc.h"
#include "NiTSet.h"
#include <NiUniversalTypes.h>
NiSmartPointer(NiMaterial);

class NiRenderer;
class NiMaterialLibraryDesc;

// NiMaterialLibrary
class NIMAIN_ENTRY NiMaterialLibrary : public NiRefObject
{
public:
    NiMaterialLibrary(const NiFixedString& kName);
    virtual ~NiMaterialLibrary();

    const NiFixedString& GetName();
    void SetName(const NiFixedString& kName);

    virtual NiMaterial* GetMaterial(const NiFixedString& kName);
    virtual const char* GetMaterialName(unsigned int ui);
    virtual unsigned int GetMaterialCount() const;

    virtual NiShaderDesc* GetFirstMaterialDesc() = 0;
    virtual NiShaderDesc* GetNextMaterialDesc() = 0;

    /// This flag modifies the behavior of the CreateMaterial function.
    /// If true (by default), then shaders that can't be created will
    /// return NULL (and will then use the default pipeline).
    /// If false, then shaders that can't be created will instead create
    /// an NiSingleShaderMaterial with the name of the missing shader.
    static bool GetCreateReturnsNullIfMissingFlag();
    static void SetCreateReturnsNullIfMissingFlag(bool bRemove);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    
    static void RemoveMaterialLibrary(NiMaterialLibrary* pkLibrary);
    static NiMaterialLibrary* GetMaterialLibrary(unsigned int uiLibrary);
    static NiMaterialLibrary* GetMaterialLibrary(const NiFixedString& kName);
    static NiMaterial* CreateMaterial(const NiFixedString& kName);
    static unsigned int GetMaterialLibraryCount();
#if defined(_USRDLL)
    static bool LoadMaterialLibraryDLL(const char* pcPath);
    static void FreeAllModules();
#endif

    // *** end Emergent internal use only ***

protected:
    NiFixedString m_kName;
   
    static NiTPrimitiveSet<NiMaterialLibrary*> ms_kMaterialLibraries;
    static bool ms_bCreateReturnsNullIfMissingFlag;

#if defined(_USRDLL)
    static NiTPrimitiveSet<NiModuleRef> ms_kModules;
#endif

};

typedef NiPointer<NiMaterialLibrary> NiMaterialLibraryPtr;

#endif  //#ifndef NIMATERIALLIBRARY_H
