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

#include "NiPSysData.h"

//---------------------------------------------------------------------------
inline NiPSysModifier* NiParticleSystem::GetModifierByName(
    const NiFixedString& kName)
{
    NIASSERT(kName.Exists());

    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter != NULL)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        if (pkModifier && pkModifier->GetName() == kName)
            return pkModifier;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiParticleSystem::GetModifierCount() const
{
    return m_kModifierList.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSysModifier* NiParticleSystem::GetModifierAt(unsigned int uiIndex)
{
    if (uiIndex >= m_kModifierList.GetSize())
    {
        return NULL;
    }

    unsigned int uiCount = 0;
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        if (uiCount++ == uiIndex)
        {
            return pkModifier;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline bool NiParticleSystem::GetWorldSpace() const
{
    return m_bWorldSpace;
}
//---------------------------------------------------------------------------
inline unsigned short NiParticleSystem::GetMaxNumParticles() const
{
    NIASSERT(NiIsKindOf(NiPSysData, m_spModelData));
    NiPSysData* pkData = NiSmartPointerCast(NiPSysData, m_spModelData);
    return pkData->GetMaxNumParticles();
}
//---------------------------------------------------------------------------
inline unsigned short NiParticleSystem::GetNumParticles() const
{
    NIASSERT(NiIsKindOf(NiPSysData, m_spModelData));
    NiPSysData* pkData = NiSmartPointerCast(NiPSysData, m_spModelData);
    return pkData->GetNumParticles();
}
//---------------------------------------------------------------------------
