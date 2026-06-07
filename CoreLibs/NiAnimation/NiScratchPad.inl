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
inline void* NiScratchPad::GetDataBlock(NiScratchPadBlock eSPBlock) const
{
    NIASSERT(eSPBlock < SPBMAXSCRATCHPADBLOCKS);
    return m_apcBlock[eSPBlock];
}
//---------------------------------------------------------------------------
inline NiUInt16 NiScratchPad::GetNumBlockItems(NiScratchPadBlock eSPBlock) 
    const
{
    NIASSERT(eSPBlock < SPBMAXSCRATCHPADBLOCKS);
    return m_ausNumBlockItems[eSPBlock];
}
//---------------------------------------------------------------------------
inline NiUInt16 NiScratchPad::GetBlockItemSize(NiScratchPadBlock eSPBlock) 
    const
{
    NIASSERT(eSPBlock < SPBMAXSCRATCHPADBLOCKS);
    NIASSERT(eSPBlock != SPBFILLDATA);
    return m_ausBlockItemSize[eSPBlock];
}
//---------------------------------------------------------------------------
inline void NiScratchPad::SetLastAccumTransform(
    const NiQuatTransform& kTransform)
{
    m_kLastAccumTransform = kTransform;
}
//---------------------------------------------------------------------------
inline void NiScratchPad::ClearAccumTransformData()
{
    // Set last time to something other than -NI_INFINITY to
    // prevent reinitialization of start accum transform.
    m_fLastAccumScaledTime = NI_INFINITY;

    m_kLastAccumTransform.SetRotate(NiQuaternion::IDENTITY);
    m_kLastAccumTransform.SetTranslate(NiPoint3::ZERO);
    m_kLastAccumTransform.SetScale(1.0f);

    m_kLoopAccumTransform.SetRotate(NiQuaternion::IDENTITY);
    m_kLoopAccumTransform.SetTranslate(NiPoint3::ZERO);
    m_kLoopAccumTransform.SetScale(1.0f);

    m_kStartAccumTransform.SetRotate(NiQuaternion::IDENTITY);
    m_kStartAccumTransform.SetTranslate(NiPoint3::ZERO);
    m_kStartAccumTransform.SetScale(1.0f);

    m_kStartAccumRot.MakeIdentity();
}
//---------------------------------------------------------------------------
