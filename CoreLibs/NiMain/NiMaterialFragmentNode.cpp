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

#include "NiMaterialFragmentNode.h"

NiImplementRTTI(NiMaterialFragmentNode, NiMaterialNode);
//---------------------------------------------------------------------------
unsigned int NiMaterialFragmentNode::GetCodeBlockCount()
{
    return m_kCodeBlocks.GetSize();
}
//---------------------------------------------------------------------------
NiCodeBlock* NiMaterialFragmentNode::GetCodeBlock(unsigned int uiCode)
{
    NIASSERT(uiCode < GetCodeBlockCount());
    return m_kCodeBlocks.GetAt(uiCode);
}
//---------------------------------------------------------------------------
NiCodeBlock* NiMaterialFragmentNode::GetCodeBlock(
    const NiFixedString& kLanguage, const NiFixedString& kPlatform,
    const NiTObjectPtrSet<NiFixedString>* pkValidTargets)
{
    for (unsigned int uiTarget = 0; uiTarget < pkValidTargets->GetSize();
        uiTarget++)
    {
        for (unsigned int ui = 0; ui < GetCodeBlockCount(); ui++)
        {
            NiCodeBlock* pkBlock = GetCodeBlock(ui);

            // There may be multiple valid platforms and languages supported by
            // a given code block. Therefore, we must search using the code 
            // block's string, not the input string.
            if (pkBlock && pkBlock->GetPlatform().ContainsNoCase(kPlatform) &&
                pkBlock->GetLanguage().ContainsNoCase(kLanguage))
            {
                // There may be multiple valid target strings in the
                // code block. Therefore, we must search using the code block's
                // string, not the input string.
                NiFixedString kTarget = pkBlock->GetTarget();
                
                if (kTarget.ContainsNoCase(pkValidTargets->GetAt(uiTarget)))
                    return pkBlock;
            }
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiMaterialFragmentNode::AddCodeBlock(NiCodeBlock* pkBlock)
{
    m_kCodeBlocks.Add(pkBlock);
}
//---------------------------------------------------------------------------
const NiString& NiMaterialFragmentNode::GetDescription()
{
    return m_kDescription;
}
//---------------------------------------------------------------------------
void NiMaterialFragmentNode::SetDescription(const NiString& kBlock)
{
    m_kDescription = kBlock;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiMaterialFragmentNode::Clone()
{
    NiMaterialFragmentNode* pkNode = NiNew NiMaterialFragmentNode();
    pkNode->CopyMembers(this);
    return pkNode;
}
//---------------------------------------------------------------------------
void NiMaterialFragmentNode::CopyMembers(NiMaterialNode* pkOther)
{
    NiMaterialNode::CopyMembers(pkOther);
    NiMaterialFragmentNode* pkFragOther = (NiMaterialFragmentNode*) pkOther;
    for (unsigned int ui = 0; ui < pkFragOther->GetCodeBlockCount(); ui++)
    {
        AddCodeBlock(pkFragOther->GetCodeBlock(ui));
    }
    m_kDescription = pkFragOther->GetDescription();
}
//---------------------------------------------------------------------------
