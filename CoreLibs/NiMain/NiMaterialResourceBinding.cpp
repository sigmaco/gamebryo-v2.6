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

#include "NiMaterialResourceBinding.h"
//---------------------------------------------------------------------------
NiMaterialResourceBinding::NiMaterialResourceBinding(
    NiMaterialResource* pkSrc, NiMaterialNode* pkSrcFrag, 
    NiMaterialResource* pkDest, NiMaterialNode* pkDestFrag)
{
    m_spSrcResource = pkSrc;
    m_spDestResource = pkDest;

    m_spSrcNode = pkSrcFrag;
    m_spDestNode = pkDestFrag;

    NIASSERT(m_spSrcNode->IsOutputResource(pkSrc));
    NIASSERT(m_spDestNode->IsInputResource(pkDest));
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialResourceBinding::GetSourceResource()
{
    return m_spSrcResource;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiMaterialResourceBinding::GetDestinationResource()
{
    return m_spDestResource;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialResourceBinding::GetSourceNode()
{
    return m_spSrcNode;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialResourceBinding::GetDestinationNode()
{
    return m_spDestNode;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResourceBinding::GetVariable()
{
    return m_kVariable;
}
//---------------------------------------------------------------------------
void NiMaterialResourceBinding::SetVariable(const NiFixedString& kVar)
{
    m_kVariable = kVar;
}
//---------------------------------------------------------------------------
