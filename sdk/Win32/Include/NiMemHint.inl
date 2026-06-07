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
inline NiMemHint::Bits NiMemHint::GetPlatform() const
{
    return (Bits)(m_eValue & 0x000000FFU);
}
//---------------------------------------------------------------------------
inline NiMemHint::Bits NiMemHint::GetLifetime() const
{
    return (Bits)(m_eValue & 0x00000300U);
}
//---------------------------------------------------------------------------
inline bool NiMemHint::CompilerProvidesSizeOnDeallocate() const
{
    return (m_eValue & COMPILER_PROVIDES_SIZE_ON_DEALLOCATE) != 0;
}
//---------------------------------------------------------------------------
inline NiMemHint::Bits NiMemHint::GetAccess() const
{
    return (Bits)(m_eValue & 0x0000F800U);
}
//---------------------------------------------------------------------------
inline NiMemHint::Bits NiMemHint::GetUsage() const
{
    return (Bits)(m_eValue & 0x00FF0000U);
}
//---------------------------------------------------------------------------
inline bool NiMemHint::IsBulk() const
{
    return (m_eValue & IS_BULK) != 0;
}
//---------------------------------------------------------------------------
inline bool NiMemHint::IsCustomHint() const
{
    return (m_eValue & TYPE_CUSTOM) != 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline unsigned int NiMemHint::GetRaw() const
{
    return (unsigned int)m_eValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline NiMemHint::NiMemHint()
{
    m_eValue = NONE;
}
//---------------------------------------------------------------------------
inline NiMemHint::NiMemHint(Bits eValue)
{
    m_eValue = eValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline NiMemHint& NiMemHint::operator|=(const NiMemHint kRhs)
{
    m_eValue = (NiMemHint::Bits)(m_eValue | kRhs.m_eValue);
    return *this;
}
//---------------------------------------------------------------------------
inline NiMemHint& NiMemHint::operator|=(const NiMemHint::Bits eRhs)
{
    m_eValue = (NiMemHint::Bits)(m_eValue | eRhs);
    return *this;
}
//---------------------------------------------------------------------------
inline NiMemHint NiMemHint::operator|(const NiMemHint kRhs) const
{
    return NiMemHint((NiMemHint::Bits)(m_eValue | kRhs.m_eValue));
}
//---------------------------------------------------------------------------
inline NiMemHint NiMemHint::operator|(const NiMemHint::Bits eRhs) const
{
    return NiMemHint((NiMemHint::Bits)(m_eValue | eRhs));
}
//---------------------------------------------------------------------------
inline bool NiMemHint::operator==(const NiMemHint kRHS) const
{
    return m_eValue == kRHS.m_eValue;
}
//---------------------------------------------------------------------------
inline bool NiMemHint::operator!=(const NiMemHint kRHS) const
{
    return m_eValue != kRHS.m_eValue;
}
//---------------------------------------------------------------------------
inline NiMemHint::Bits operator|(const NiMemHint::Bits eLhs, const NiMemHint::Bits eRhs)
{
    return (NiMemHint::Bits)((unsigned int)eLhs | (unsigned int)eRhs);
}
//---------------------------------------------------------------------------
