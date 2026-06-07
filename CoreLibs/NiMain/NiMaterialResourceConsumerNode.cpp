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

#include "NiMaterialResourceConsumerNode.h"

NiImplementRTTI(NiMaterialResourceConsumerNode, NiMaterialNode);
//---------------------------------------------------------------------------
NiMaterialResourceConsumerNode::NiMaterialResourceConsumerNode(
    const NiFixedString& kName, const NiFixedString& kConsumerType) :
    NiMaterialNode()
{
    SetName(kName);
    m_kConsumerType = kConsumerType;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialResourceConsumerNode::GetConsumerType()
{
    return m_kConsumerType;
}
//---------------------------------------------------------------------------

