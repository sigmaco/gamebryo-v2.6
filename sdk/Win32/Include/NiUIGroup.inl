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

//---------------------------------------------------------------------------
// NiUIGroup inline function
//---------------------------------------------------------------------------
#include "NiUIButton.h"
#include "NiUIImageButton.h"

//---------------------------------------------------------------------------
inline unsigned int NiUIGroup::NumQuadsRequired()
{
    return NUM_QUADS;
}
//---------------------------------------------------------------------------
inline void NiUIGroup::SetVisible(bool bVisible)
{
    NiUIRenderGroup::SetVisible(bVisible);
    if (bVisible)
        Expand(m_bExpanded);
}
//---------------------------------------------------------------------------
inline void NiUIGroup::SubscribeToExpandToggleEvent(
    NiUIBaseSlot1<bool>* pkSlot)
{
    m_spExpandButton->SubscribeToToggleEvent(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIGroup::SetExpandState(bool bExpand)
{
    m_spExpandButton->SetState(bExpand);
}
//---------------------------------------------------------------------------
