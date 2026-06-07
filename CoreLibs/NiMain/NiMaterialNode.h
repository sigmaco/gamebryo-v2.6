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

#ifndef NIMATERIALNODE_H
#define NIMATERIALNODE_H

#include "NiMaterialResource.h"
#include "NiTArray.h"
#include <NiRTTI.h>

class NIMAIN_ENTRY NiMaterialNode : public NiRefObject
{
    NiDeclareRootRTTI(NiMaterialNode);
public:
    NiMaterialNode();
    virtual ~NiMaterialNode();

    const NiFixedString& GetName();

    void SetName(const NiFixedString& kName);
    const NiFixedString& GetType();
    void SetType(const NiFixedString& kType);

    unsigned int GetInputResourceCount();
    NiMaterialResource* GetInputResource(unsigned int uiRes);
    bool SetInputResourceAt(unsigned int uiIndex, NiMaterialResource* pkRes);
    NiMaterialResource* GetInputResourceByVariableName(
        const NiFixedString& kString);
    void AddInputResource(NiMaterialResource* pkResource);
    NiMaterialResource* AddInputResource(
        const NiFixedString& kType, 
        const NiFixedString& kResSemantic, const NiFixedString& kResLabel, 
        const NiFixedString& kResVariable, unsigned int uiCount = 1,
        NiMaterialResource::Source eSource = 
        NiMaterialResource::SOURCE_UNKNOWN,
        NiShaderAttributeDesc::ObjectType eType = 
        NiShaderAttributeDesc::OT_UNDEFINED,
        unsigned int uiExtraData = 0);

    unsigned int GetOutputResourceCount();
    NiMaterialResource* GetOutputResource(unsigned int uiRes);
    bool SetOutputResourceAt(unsigned int uiIndex, NiMaterialResource* pkRes);
    NiMaterialResource* GetOutputResourceByVariableName(
        const NiFixedString& kString);
    void AddOutputResource(NiMaterialResource* pkResource);
    NiMaterialResource* AddOutputResource(
        const NiFixedString& kType, 
        const NiFixedString& kResSemantic, const NiFixedString& kResLabel, 
        const NiFixedString& kResVariable, unsigned int uiCount = 1,
        NiMaterialResource::Source eSource = 
        NiMaterialResource::SOURCE_UNKNOWN,
        NiShaderAttributeDesc::ObjectType eType = 
        NiShaderAttributeDesc::OT_UNDEFINED,
        unsigned int uiExtraData = 0);
    NiMaterialResource* AddOutputConstant(
        const NiFixedString& kType, 
        const NiFixedString& kDefaultValue);

    virtual bool ContainsResource(NiMaterialResource* pkRes);
    virtual bool IsOutputResource(NiMaterialResource* pkRes);
    virtual bool IsInputResource(NiMaterialResource* pkRes);

    virtual NiMaterialNode* Clone();

protected:
    virtual void CopyMembers(NiMaterialNode* pkOther);

    NiFixedString m_kType;
    NiFixedString m_kName;

    NiTObjectArray<NiMaterialResourcePtr> m_kInputs;
    NiTObjectArray<NiMaterialResourcePtr> m_kOutputs;

};

typedef NiPointer<NiMaterialNode> NiMaterialNodePtr;

#endif  //#ifndef NIMATERIALNODE_H
