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
//
// THIS CLASS HAS BEEN DEPRECATED.
// Functionality here exists only to enable streaming and conversion to the
// NiMorphMeshModifier class.
//
//---------------------------------------------------------------------------


#include <NiGeometry.h>

//---------------------------------------------------------------------------
inline NiGeomMorpherController::InterpItem::InterpItem() : m_fWeight(0.0f)
{
}
//---------------------------------------------------------------------------
inline NiMorphData* NiGeomMorpherController::GetMorphData() const
{
    return m_spMorphData;
}
//---------------------------------------------------------------------------
inline unsigned int NiGeomMorpherController::GetNumTargets() const
{
    return m_spMorphData ? m_spMorphData->GetNumTargets() : 0;
}
//---------------------------------------------------------------------------
inline bool NiGeomMorpherController::GetUpdateNormals() const
{
    return GetBit(UPDATENORMAL_MASK);
}
//---------------------------------------------------------------------------
inline bool NiGeomMorpherController::GetAlwaysUpdate() const
{
    return m_bAlwaysUpdate;
}
//---------------------------------------------------------------------------
inline bool NiGeomMorpherController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiGeometry,m_pkTarget);
}
//---------------------------------------------------------------------------
inline float NiGeomMorpherController::GetWeight(unsigned int uiIndex)
{
    float fWeight = 0.0f;

    if (m_pkInterpItems)
    {
        NIASSERT(uiIndex < GetNumTargets());
        fWeight = m_pkInterpItems[uiIndex].m_fWeight;
    }

    return fWeight;
}
//---------------------------------------------------------------------------
inline void NiGeomMorpherController::AllocateInterpArray()
{
    NiDelete[] m_pkInterpItems;
    m_pkInterpItems = NULL;

    if (m_spMorphData && m_spMorphData->GetNumTargets() > 0)
    {
        m_pkInterpItems = NiNew InterpItem[m_spMorphData->GetNumTargets()];
    }
}
//---------------------------------------------------------------------------


