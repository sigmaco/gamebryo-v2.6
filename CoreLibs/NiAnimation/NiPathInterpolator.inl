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
inline NiPosData* NiPathInterpolator::GetPathData() const
{
    return m_spPathData;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetPathData(NiPosData* pPathData)
{
    m_spPathData = pPathData;
}
//---------------------------------------------------------------------------
inline NiFloatData* NiPathInterpolator::GetPctData() const
{
    return m_spPctData;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetPctData(NiFloatData* pPctData)
{
    m_spPctData = pPctData;
}
//---------------------------------------------------------------------------
inline NiPosKey* NiPathInterpolator::GetPathKeys(unsigned int& uiNumKeys,
    NiPosKey::KeyType& eType, unsigned char& ucSize) const
{
    NiPosKey* pkPathKeys = NULL;
    if (m_spPathData)
    {
        pkPathKeys = m_spPathData->GetAnim(uiNumKeys, eType, ucSize);
    }
    else
    {
        uiNumKeys = 0;
        eType = NiAnimationKey::NOINTERP;
        ucSize = 0;
    }
    return pkPathKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiPathInterpolator::GetPctKeys(unsigned int& uiNumKeys,
    NiFloatKey::KeyType& eType, unsigned char& ucSize) const
{
    NiFloatKey* pkPctKeys = NULL;
    if (m_spPctData)
    {
        pkPctKeys = m_spPctData->GetAnim(uiNumKeys, eType, ucSize);
    }
    else
    {
        uiNumKeys = 0;
        eType = NiAnimationKey::NOINTERP;
        ucSize = 0;
    }
    return pkPctKeys;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetAllowFlip(bool bAllowFlip)
{
    SetBit(bAllowFlip, ALLOWFLIP_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetAllowFlip() const
{
    return GetBit(ALLOWFLIP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetBank(bool bBank)
{
    SetBit(bBank, BANK_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetBank() const
{
    return GetBit(BANK_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetBankDir(BankDir eDir)
{
    m_eBankDir = eDir;
}
//---------------------------------------------------------------------------
inline NiPathInterpolator::BankDir NiPathInterpolator::GetBankDir() const
{
    return m_eBankDir;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetConstVelocity(bool bConstVelocity)
{
    SetBit(bConstVelocity, CONSTVELOCITY_MASK);
    if (bConstVelocity)
        SetConstantVelocityData();
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetConstVelocity() const
{
    return GetBit(CONSTVELOCITY_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetFollow(bool bFollow)
{
    SetBit(bFollow, FOLLOW_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetFollow() const
{
    return GetBit(FOLLOW_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetMaxBankAngle(float fAngle)
{
    m_fMaxBankAngle = fAngle;
}
//---------------------------------------------------------------------------
inline float NiPathInterpolator::GetMaxBankAngle() const
{
    return m_fMaxBankAngle;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetSmoothing(float fSmooth)
{
    m_fSmoothing = fSmooth;
}
//---------------------------------------------------------------------------
inline float NiPathInterpolator::GetSmoothing() const
{
    return m_fSmoothing;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetFollowAxis(short sAxis)
{
    m_sFollowAxis = sAxis;
}
//---------------------------------------------------------------------------
inline short NiPathInterpolator::GetFollowAxis() const
{
    return m_sFollowAxis;
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetFlip(bool bFlip)
{
    SetBit(bFlip, FLIP_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetFlip() const
{
    return GetBit(FLIP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetCurveTypeOpen(bool bOpen)
{
    SetBit(bOpen, OPENCURVE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetCurveTypeOpen() const
{
    return GetBit(OPENCURVE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathInterpolator::SetCVDataNeedsUpdate(bool bUpdate)
{
    SetBit(bUpdate, CVDATAUPDATE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathInterpolator::GetCVDataNeedsUpdate() const
{
    return GetBit(CVDATAUPDATE_MASK);
}
//---------------------------------------------------------------------------
