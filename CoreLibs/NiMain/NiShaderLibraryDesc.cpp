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

#include "NiShaderLibraryDesc.h"
#include <NiShader.h>

//---------------------------------------------------------------------------
// Description of the shader library
//---------------------------------------------------------------------------
NiShaderLibraryDesc::NiShaderLibraryDesc() :
      NiShaderRequirementDesc()
{
    m_kShaderDescs.RemoveAll();
}
//---------------------------------------------------------------------------
NiShaderLibraryDesc::~NiShaderLibraryDesc()
{
    // The desc is responsible for deleting the name
    for (unsigned int ui = 0; ui < m_kShaderDescs.GetSize(); ui++)
    {
        m_kShaderDescs.SetAt(ui, 0);
    }
    m_kShaderDescs.RemoveAll();

}
//---------------------------------------------------------------------------
bool NiShaderLibraryDesc::AddShaderDesc(NiShaderDesc* pkDesc)
{
    if (!pkDesc)
        return false;

    m_kShaderDescs.AddFirstEmpty(pkDesc);

    return true;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderLibraryDesc::GetShaderCount() const
{
    return m_kShaderDescs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
const char* NiShaderLibraryDesc::GetShaderName(unsigned int uiShader) const
{
    NiShaderDesc* pkDesc = GetShaderDesc(uiShader);
    if (pkDesc)
        return pkDesc->GetName();

    return 0;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiShaderLibraryDesc::GetShaderDesc(unsigned int uiShader) const
{
    return m_kShaderDescs.GetAt(uiShader);
}
//---------------------------------------------------------------------------
NiShaderDesc* NiShaderLibraryDesc::GetShaderDesc(const char* pszShaderName) 
    const
{
    unsigned int uiCount = m_kShaderDescs.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiShaderDesc* pkDesc = m_kShaderDescs.GetAt(ui);
        if (pkDesc)
        {
            if (strcmp(pszShaderName, pkDesc->GetName()) == 0)
                return pkDesc;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
