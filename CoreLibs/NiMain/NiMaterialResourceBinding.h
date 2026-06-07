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

#ifndef NIMATERIALRESOURCEBINDING_H
#define NIMATERIALRESOURCEBINDING_H

#include "NiRefObject.h"
#include "NiMaterialNode.h"
#include "NiMainLibType.h"

class NIMAIN_ENTRY NiMaterialResourceBinding : public NiRefObject
{
public:
    NiMaterialResourceBinding(NiMaterialResource* pkSrc, 
        NiMaterialNode* pkSrcFrag, NiMaterialResource* pkDest, 
        NiMaterialNode* pkDestFrag);

    NiMaterialResource* GetSourceResource();
    NiMaterialResource* GetDestinationResource();

    NiMaterialNode* GetSourceNode();
    NiMaterialNode* GetDestinationNode();

    const NiFixedString& GetVariable();
    void SetVariable(const NiFixedString& kVar);
protected:
    NiMaterialResourcePtr m_spSrcResource;
    NiMaterialResourcePtr m_spDestResource;
    NiMaterialNodePtr m_spSrcNode;
    NiMaterialNodePtr m_spDestNode;
    NiFixedString m_kVariable;
};

typedef NiPointer<NiMaterialResourceBinding> NiMaterialResourceBindingPtr;

#endif  //#ifndef NIMATERIALRESOURCEBINDING_H
