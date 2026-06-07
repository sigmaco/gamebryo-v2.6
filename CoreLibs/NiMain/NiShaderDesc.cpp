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

#include "NiShaderDesc.h"

//---------------------------------------------------------------------------
// Description of a shader
//---------------------------------------------------------------------------
NiShaderDesc::NiShaderDesc() :
    NiShaderRequirementDesc(), 
    m_uiImplementations(0)
{
    m_kImplementationDescs.RemoveAll();
    m_kAttributeList.RemoveAll();
}
//---------------------------------------------------------------------------
NiShaderDesc::~NiShaderDesc()
{
    // The desc is responsible for deleting the strings
    for (unsigned int ui = 0; ui < m_kImplementationDescs.GetSize(); ui++)
    {
        NiShaderRequirementDesc* pkDesc = m_kImplementationDescs.GetAt(ui);
        if (pkDesc)
        {
            m_kImplementationDescs.SetAt(ui, 0);
            NiDelete pkDesc;
        }
    }
    m_kImplementationDescs.RemoveAll();

    m_kAttributeList.RemoveAll();
}
//---------------------------------------------------------------------------
const unsigned int NiShaderDesc::GetNumberOfImplementations() const
{
    return m_uiImplementations;
}
//---------------------------------------------------------------------------
const NiShaderRequirementDesc* NiShaderDesc::GetImplementationDescription(
    unsigned int uiImplementation) const
{
    return m_kImplementationDescs.GetAt(uiImplementation);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderDesc::GetNumberOfAttributes() const
{
    return m_kAttributeList.GetSize();
}
//---------------------------------------------------------------------------
const NiShaderAttributeDesc* NiShaderDesc::GetFirstAttribute() const
{
    m_kAttributePos = m_kAttributeList.GetHeadPos();
    if (m_kAttributePos)
        return m_kAttributeList.GetNext(m_kAttributePos);
    return NULL;
}
//---------------------------------------------------------------------------
const NiShaderAttributeDesc* NiShaderDesc::GetNextAttribute() const
{
    if (m_kAttributePos)
        return m_kAttributeList.GetNext(m_kAttributePos);
    return NULL;
}
//---------------------------------------------------------------------------
const NiShaderAttributeDesc* NiShaderDesc::GetAttribute(const char* pcName)
    const
{
    if (!pcName)
        return NULL;

    const NiShaderAttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        if (strcmp(pkDesc->GetName(), pcName) == 0)
            return pkDesc;

        pkDesc = GetNextAttribute();
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiShaderDesc::SetNumberOfImplementations(unsigned int uiCount)
{
    m_uiImplementations = uiCount;
}
//---------------------------------------------------------------------------
void NiShaderDesc::SetImplementationDescription(unsigned int uiImplementation,
    NiShaderRequirementDesc* pkReqDesc)
{
    NIASSERT(pkReqDesc);
    m_kImplementationDescs.SetAtGrow(uiImplementation, pkReqDesc);
}
//---------------------------------------------------------------------------
void NiShaderDesc::AddAttribute(NiShaderAttributeDesc* pkAttribute)
{
    if (!pkAttribute)
        return;

    m_kAttributeList.AddTail(pkAttribute);
}
//---------------------------------------------------------------------------
void NiShaderDesc::RemoveAttribute(NiShaderAttributeDesc* pkAttribute)
{
    if (!pkAttribute)
        return;

    m_kAttributeList.Remove(pkAttribute);
}
//---------------------------------------------------------------------------
void NiShaderDesc::RemoveAttribute(const char* pcName)
{
    NiShaderAttributeDesc* pkDesc = 
        (NiShaderAttributeDesc*)GetAttribute(pcName);
    if (pkDesc)
        m_kAttributeList.Remove(pkDesc);
}
//---------------------------------------------------------------------------
void NiShaderDesc::RemoveAllAttributes()
{
    m_kAttributeList.RemoveAll();
}
//---------------------------------------------------------------------------
