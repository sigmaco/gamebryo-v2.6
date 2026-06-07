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

#ifndef NIMATERIALRESOURCE_H
#define NIMATERIALRESOURCE_H

#include "NiFixedString.h"
#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiShaderAttributeDesc.h"

class NIMAIN_ENTRY NiMaterialResource : public NiRefObject
{
public:
    enum Source
    {
        SOURCE_UNKNOWN = 0,
        SOURCE_OBJECT,
        SOURCE_PREDEFINED,
        SOURCE_CONSTANT,
        SOURCE_ATTRIBUTE,
        SOURCE_GLOBAL,
        SOURCE_MAX
    };

    NiMaterialResource();
    const NiFixedString& GetType();
    const NiFixedString& GetSemantic();
    const NiFixedString& GetLabel();
    const NiFixedString& GetVariable();
    const NiFixedString& GetDefaultValue();
    unsigned int GetCount();
    Source GetSource();
    unsigned int GetExtraData();
    NiShaderAttributeDesc::ObjectType GetObjectType();

    void SetType(const NiFixedString& eType);
    void SetSemantic(const NiFixedString& kSemantic);
    void SetLabel(const NiFixedString& kLabel);
    void SetVariable(const NiFixedString& kVariable);
    void SetDefaultValue(const NiFixedString& kValue);
    void SetCount(unsigned int uiCount);
    void SetSource(Source eSource);
    void SetExtraData(unsigned int uiExtra);
    void SetObjectType(NiShaderAttributeDesc::ObjectType eType);

    NiMaterialResource* Clone();
protected:
    NiFixedString m_kType;
    NiFixedString m_kSemantic;
    NiFixedString m_kLabel;
    NiFixedString m_kVariable;
    NiFixedString m_kDefaultValue;
    unsigned int m_uiCount;
    Source m_eSource;
    unsigned int m_uiExtraData;
    NiShaderAttributeDesc::ObjectType m_eObjectType;
};
//---------------------------------------------------------------------------
typedef NiPointer<NiMaterialResource> NiMaterialResourcePtr;

#endif  //#ifndef NIMATERIALRESOURCE_H
