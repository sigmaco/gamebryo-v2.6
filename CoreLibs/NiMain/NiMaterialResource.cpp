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

#include "NiMaterialResource.h"
//---------------------------------------------------------------------------
NiMaterialResource::NiMaterialResource() : 
    m_uiCount(1), m_eSource(SOURCE_UNKNOWN), m_uiExtraData(0), 
    m_eObjectType(NiShaderAttributeDesc::OT_UNDEFINED)
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResource::GetType()
{
    return m_kType;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResource::GetSemantic()
{
    return m_kSemantic;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResource::GetLabel()
{
    return m_kLabel;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResource::GetVariable()
{
    return m_kVariable;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResource::GetDefaultValue()
{
    return m_kDefaultValue;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialResource::GetCount()
{
    return m_uiCount;
}
//---------------------------------------------------------------------------
NiMaterialResource::Source NiMaterialResource::GetSource()
{
    return m_eSource;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialResource::GetExtraData()
{
    return m_uiExtraData;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::ObjectType NiMaterialResource::GetObjectType()
{
    return m_eObjectType;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetType(const NiFixedString& kType)
{
    m_kType = kType;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetSemantic(const NiFixedString& kSemantic)
{
    m_kSemantic = kSemantic;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetLabel(const NiFixedString& kLabel)
{
    m_kLabel = kLabel;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetVariable(const NiFixedString& kVariable)
{
    m_kVariable = kVariable;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetDefaultValue(const NiFixedString& kValue)
{
    m_kDefaultValue = kValue;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetCount(unsigned int uiCount)
{
    NIASSERT(uiCount > 0);
    m_uiCount = uiCount;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetSource(Source eSource)
{
    m_eSource = eSource;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetExtraData(unsigned int uiExtraData)
{
    m_uiExtraData = uiExtraData;
}
//---------------------------------------------------------------------------
void NiMaterialResource::SetObjectType(NiShaderAttributeDesc::ObjectType eType)
{
    m_eObjectType = eType;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialResource::Clone()
{
    NiMaterialResource* pkRes = NiNew NiMaterialResource();
    pkRes->m_kType = m_kType;
    pkRes->m_kSemantic = m_kSemantic;
    pkRes->m_kLabel = m_kLabel;
    pkRes->m_kVariable = m_kVariable;
    pkRes->m_uiCount = m_uiCount;
    pkRes->m_kDefaultValue = m_kDefaultValue;
    pkRes->m_eSource = m_eSource;
    pkRes->m_eObjectType = m_eObjectType;
    pkRes->m_uiExtraData = m_uiExtraData;
    return pkRes;
}
//---------------------------------------------------------------------------

