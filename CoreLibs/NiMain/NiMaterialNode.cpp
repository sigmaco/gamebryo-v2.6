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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiMaterialNode.h"

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiMaterialNode);
//---------------------------------------------------------------------------
NiMaterialNode::NiMaterialNode()
{
}
//---------------------------------------------------------------------------
NiMaterialNode::~NiMaterialNode()
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialNode::GetName()
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiMaterialNode::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialNode::GetInputResourceCount()
{
    return m_kInputs.GetSize();
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::GetInputResource(unsigned int uiRes)
{
    NIASSERT(uiRes < GetInputResourceCount());
    return m_kInputs.GetAt(uiRes);
}
//---------------------------------------------------------------------------
bool NiMaterialNode::SetInputResourceAt(unsigned int uiIndex, 
    NiMaterialResource* pkRes)
{
    if(uiIndex >= GetInputResourceCount())
        return false;
    
    m_kInputs.SetAt(uiIndex, pkRes);

    return true;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::GetInputResourceByVariableName(
    const NiFixedString& kString)
{
    for (unsigned int ui = 0; ui < GetInputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes = GetInputResource(ui);
        if (pkRes && pkRes->GetVariable() == kString)
            return pkRes;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiMaterialNode::AddInputResource(NiMaterialResource* spResource)
{
    m_kInputs.Add(spResource);
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::AddInputResource(
    const NiFixedString& kType, 
    const NiFixedString& kResSemantic, const NiFixedString& kResLabel, 
    const NiFixedString& kResVariable, unsigned int uiCount,
    NiMaterialResource::Source eSource, 
    NiShaderAttributeDesc::ObjectType eObjectType, unsigned int uiExtraData)
{
    NIASSERT(!GetInputResourceByVariableName(kResVariable));
    NiMaterialResource* pkRes = NiNew NiMaterialResource();
    pkRes->SetType(kType);
    pkRes->SetVariable(kResVariable);
    pkRes->SetLabel(kResLabel);
    pkRes->SetSemantic(kResSemantic);
    pkRes->SetCount(uiCount);
    pkRes->SetSource(eSource);
    pkRes->SetExtraData(uiExtraData);
    pkRes->SetObjectType(eObjectType);
    m_kInputs.Add(pkRes);
    return pkRes;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialNode::GetOutputResourceCount()
{
    return m_kOutputs.GetSize();
}
//---------------------------------------------------------------------------
NiMaterialResource*
NiMaterialNode::GetOutputResource(unsigned int uiRes)
{
    NIASSERT(uiRes < GetOutputResourceCount());
    return m_kOutputs.GetAt(uiRes);
}
//---------------------------------------------------------------------------
bool NiMaterialNode::SetOutputResourceAt(unsigned int uiIndex, 
                                        NiMaterialResource* pkRes)
{
    if(uiIndex >= GetOutputResourceCount())
        return false;

    m_kOutputs.SetAt(uiIndex, pkRes);

    return true;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::AddOutputResource(
    const NiFixedString& kType, 
    const NiFixedString& kResSemantic, const NiFixedString& kResLabel, 
    const NiFixedString& kResVariable, unsigned int uiCount,
    NiMaterialResource::Source eSource, 
    NiShaderAttributeDesc::ObjectType eObjectType, unsigned int uiExtraData)
{
    NiMaterialResource* pkRes = GetOutputResourceByVariableName(kResVariable);
    if (pkRes)
    {
        // Ensure that the output resource that already exists is the same as
        // the output resource we wanted to create.
        NIASSERT(pkRes->GetType() == kType);
        NIASSERT(pkRes->GetVariable() == kResVariable);
        NIASSERT(pkRes->GetLabel() == kResLabel);
        NIASSERT(pkRes->GetSemantic() == kResSemantic);
        NIASSERT(pkRes->GetCount() == uiCount);
        NIASSERT(pkRes->GetSource() == eSource);
        NIASSERT(pkRes->GetExtraData() == uiExtraData);
        NIASSERT(pkRes->GetObjectType() == eObjectType);
        return pkRes;
    }
    

    pkRes = NiNew NiMaterialResource();
    pkRes->SetType(kType);
    pkRes->SetVariable(kResVariable);
    pkRes->SetLabel(kResLabel);
    pkRes->SetSemantic(kResSemantic);
    pkRes->SetCount(uiCount);
    pkRes->SetSource(eSource);
    pkRes->SetExtraData(uiExtraData);
    pkRes->SetObjectType(eObjectType);
    m_kOutputs.Add(pkRes);
    return pkRes;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::AddOutputConstant(
    const NiFixedString& kType,
    const NiFixedString& kDefaultValue)
{
    NiMaterialResource* pkRes = NiNew NiMaterialResource();
    pkRes->SetType(kType);
    pkRes->SetDefaultValue(kDefaultValue);
    pkRes->SetSource(NiMaterialResource::SOURCE_CONSTANT);
    m_kOutputs.Add(pkRes);
    return pkRes;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialNode::Clone()
{
    NiMaterialNode* pkNode = NiNew NiMaterialNode();
    pkNode->CopyMembers(this);
    return pkNode;
}
//---------------------------------------------------------------------------
void NiMaterialNode::CopyMembers(NiMaterialNode* pkOther)
{
    m_kType = pkOther->GetType();
    m_kName = pkOther->GetName();

    for (unsigned int ui = 0; ui < pkOther->GetInputResourceCount();ui++)
    {
        NiMaterialResource* pkRes = pkOther->GetInputResource(ui);
        AddInputResource(pkRes->Clone());
    }
    for (unsigned int ui = 0; ui < pkOther->GetOutputResourceCount();ui++)
    {
        NiMaterialResource* pkRes = pkOther->GetOutputResource(ui);
        AddOutputResource(pkRes->Clone());
    }
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialNode::GetOutputResourceByVariableName(
    const NiFixedString& kString)
{
    for (unsigned int ui = 0; ui < GetOutputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes = GetOutputResource(ui);
        if (pkRes && pkRes->GetVariable() == kString)
            return pkRes;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiMaterialNode::AddOutputResource(NiMaterialResource* kResource)
{
    m_kOutputs.Add(kResource);
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialNode::GetType()
{
    return m_kType;
}
//---------------------------------------------------------------------------
void NiMaterialNode::SetType(const NiFixedString& kStr)
{
    m_kType = kStr;
}
//---------------------------------------------------------------------------
bool NiMaterialNode::ContainsResource(NiMaterialResource* pkRes)
{
    if (IsInputResource(pkRes) || IsOutputResource(pkRes))
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiMaterialNode::IsOutputResource(NiMaterialResource* pkRes)
{
    for (unsigned int ui = 0; ui < GetOutputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes2 = GetOutputResource(ui);
        if (pkRes2 && pkRes2 == pkRes)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMaterialNode::IsInputResource(NiMaterialResource* pkRes)
{
    for (unsigned int ui = 0; ui < GetInputResourceCount(); ui++)
    {
        NiMaterialResource* pkRes2 = GetInputResource(ui);
        if (pkRes2 && pkRes2 == pkRes)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
