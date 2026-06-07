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
inline NiSkinData::NiSkinData()
    : m_pkBoneData(0)
{
    // Used for streaming. All members will be initialized by LoadBinary.
}

//---------------------------------------------------------------------------
inline unsigned int NiSkinData::GetBoneCount() const
{
    return m_uiBones;
}

//---------------------------------------------------------------------------
inline NiSkinData::BoneData* NiSkinData::GetBoneData() const
{
    return m_pkBoneData;
}

//---------------------------------------------------------------------------
inline const NiTransform& NiSkinData::GetRootParentToSkin() const
{
    return m_kRootParentToSkin;
}
//---------------------------------------------------------------------------
inline NiSkinPartition* NiSkinData::GetSkinPartition(bool bDontAssert) const
{
    if (bDontAssert)
        return m_spSkinPartition;
    NIASSERT(false);
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiSkinData::SetSkinPartition(NiSkinPartition* pkSkinPartition,
    bool bDontAssert)
{
    if (bDontAssert)
        m_spSkinPartition = pkSkinPartition;
    else
        NIASSERT(false);
}
//---------------------------------------------------------------------------
