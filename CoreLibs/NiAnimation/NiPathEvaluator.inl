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
inline NiPosData* NiPathEvaluator::GetPathData() const
{
    return m_spPathData;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetPathData(NiPosData* pPathData)
{
    m_spPathData = pPathData;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiFloatData* NiPathEvaluator::GetPctData() const
{
    return m_spPctData;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetPctData(NiFloatData* pPctData)
{
    m_spPctData = pPctData;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiPosKey* NiPathEvaluator::GetPathKeys(unsigned int& uiNumKeys,
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
inline NiFloatKey* NiPathEvaluator::GetPctKeys(unsigned int& uiNumKeys,
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
inline void NiPathEvaluator::SetAllowFlip(bool bAllowFlip)
{
    SetBit(bAllowFlip, ALLOWFLIP_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetAllowFlip() const
{
    return GetBit(ALLOWFLIP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetBank(bool bBank)
{
    SetBit(bBank, BANK_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetBank() const
{
    return GetBit(BANK_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetBankDir(BankDir eDir)
{
    m_eBankDir = eDir;
}
//---------------------------------------------------------------------------
inline NiPathEvaluator::BankDir NiPathEvaluator::GetBankDir() const
{
    return m_eBankDir;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetConstVelocity(bool bConstVelocity)
{
    SetBit(bConstVelocity, CONSTVELOCITY_MASK);
    if (bConstVelocity)
    {
        SetConstantVelocityData();
    }
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetConstVelocity() const
{
    return GetBit(CONSTVELOCITY_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetFollow(bool bFollow)
{
    SetBit(bFollow, FOLLOW_MASK);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetFollow() const
{
    return GetBit(FOLLOW_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetMaxBankAngle(float fAngle)
{
    m_fMaxBankAngle = fAngle;
}
//---------------------------------------------------------------------------
inline float NiPathEvaluator::GetMaxBankAngle() const
{
    return m_fMaxBankAngle;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetSmoothing(float fSmooth)
{
    m_fSmoothing = fSmooth;
}
//---------------------------------------------------------------------------
inline float NiPathEvaluator::GetSmoothing() const
{
    return m_fSmoothing;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetFollowAxis(short sAxis)
{
    m_sFollowAxis = sAxis;
}
//---------------------------------------------------------------------------
inline short NiPathEvaluator::GetFollowAxis() const
{
    return m_sFollowAxis;
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetFlip(bool bFlip)
{
    SetBit(bFlip, FLIP_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetFlip() const
{
    return GetBit(FLIP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetCurveTypeOpen(bool bOpen)
{
    SetBit(bOpen, OPENCURVE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetCurveTypeOpen() const
{
    return GetBit(OPENCURVE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPathEvaluator::SetCVDataNeedsUpdate(bool bUpdate)
{
    SetBit(bUpdate, CVDATAUPDATE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPathEvaluator::GetCVDataNeedsUpdate() const
{
    return GetBit(CVDATAUPDATE_MASK);
}
//---------------------------------------------------------------------------
