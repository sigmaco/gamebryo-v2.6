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

#ifndef NISHADERDESC_H
#define NISHADERDESC_H

// These classes and functions are used for DLL-based shader libraries
#include "NiShaderRequirementDesc.h"
#include "NiShaderAttributeDesc.h"
#include "NiTArray.h"
#include "NiTPointerList.h"
#include "NiTStringMap.h"
#include "NiRefObject.h"
#include "NiSmartPointer.h"

// Description of a shader
class NIMAIN_ENTRY NiShaderDesc : public NiShaderRequirementDesc
{
public:
    NiShaderDesc();
    virtual ~NiShaderDesc();

    const unsigned int GetNumberOfImplementations() const;
    const NiShaderRequirementDesc* GetImplementationDescription(
        unsigned int uiImplementation) const;
    // Attribute listings
    const unsigned int GetNumberOfAttributes() const;
    const NiShaderAttributeDesc* GetFirstAttribute() const;
    const NiShaderAttributeDesc* GetNextAttribute() const;

    const NiShaderAttributeDesc* GetAttribute(const char* pcName) const;

    // *** begin Emergent internal use only ***
    void SetNumberOfImplementations(unsigned int uiCount);
    void SetImplementationDescription(unsigned int uiImplementation,
        NiShaderRequirementDesc* pkReqDesc);
    void AddAttribute(NiShaderAttributeDesc* pkAttribute);
    void RemoveAttribute(NiShaderAttributeDesc* pkAttribute);
    void RemoveAttribute(const char* pcName);
    void RemoveAllAttributes();
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiImplementations;
    NiTPrimitiveArray<NiShaderRequirementDesc*> m_kImplementationDescs;
    mutable NiTListIterator m_kAttributePos;
    NiTPointerList<NiShaderAttributeDescPtr> m_kAttributeList;
};

typedef NiPointer<NiShaderDesc> NiShaderDescPtr;

#endif  //#ifndef NISHADERDESC_H
