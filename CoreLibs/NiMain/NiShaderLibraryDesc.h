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

#ifndef NISHADERLIBRARYDESC_H
#define NISHADERLIBRARYDESC_H

// These classes and functions are used for DLL-based shader libraries
#include "NiShaderDesc.h"

// Description of the shader library
class NIMAIN_ENTRY NiShaderLibraryDesc : public NiShaderRequirementDesc
{
public:
    NiShaderLibraryDesc();
    virtual ~NiShaderLibraryDesc();

    const unsigned int GetShaderCount() const;
    const char* GetShaderName(unsigned int uiShader) const;
    NiShaderDesc* GetShaderDesc(unsigned int uiShader) const;
    NiShaderDesc* GetShaderDesc(const char* pszShaderName) const;

    // *** begin Emergent internal use only ***
    bool AddShaderDesc(NiShaderDesc* pkDesc);
    // *** end Emergent internal use only ***

protected:
    NiTObjectArray<NiShaderDescPtr> m_kShaderDescs;
};

typedef NiPointer<NiShaderLibraryDesc> NiShaderLibraryDescPtr;

#endif  //#ifndef NISHADERLIBRARYDESC_H
