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

#include <NiSystemDesc.h>
#include <NiEndian.h>
//---------------------------------------------------------------------------
inline NiPixelFormat::Format NiPixelFormat::GetFormat() const
{
    return m_eFormat;
}
//---------------------------------------------------------------------------
inline unsigned char NiPixelFormat::GetBitsPerPixel() const
{
    return m_ucBitsPerPixel;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelFormat::GetRendererHint() const
{
    return m_uiRendererHint;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelFormat::GetExtraData() const
{
    return m_uiExtraData;
}
//---------------------------------------------------------------------------
inline NiPixelFormat::Tiling NiPixelFormat::GetTiling() const
{
    return m_eTiling;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetLittleEndian() const
{
    return GetBit(ENDIAN_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetSRGBSpace() const
{
    return m_bSRGBSpace;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetFormat(NiPixelFormat::Format eFormat)
{
    m_eFormat = eFormat;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetBitsPerPixel(unsigned char ucBPP)
{
    m_ucBitsPerPixel = ucBPP;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetRendererHint(unsigned int uiRendererHint)
{
    m_uiRendererHint = uiRendererHint;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetExtraData(unsigned int uiExtraData)
{
    m_uiExtraData = uiExtraData;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetTiling(NiPixelFormat::Tiling eTiling)
{
    m_eTiling = eTiling;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetLittleEndian(bool bLittleEndian)
{
    SetBit(bLittleEndian, ENDIAN_MASK);
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetSRGBSpace(bool bSRGBSpace)
{
    m_bSRGBSpace = bSRGBSpace;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetComponent(unsigned int uiWhichComponent, 
    NiPixelFormat::Component& eComponent, NiPixelFormat::Representation& eRep,
    unsigned char& ucBPC, bool& bSigned) const
{
    NIASSERT(uiWhichComponent < NUM_COMPS);
    bSigned = m_akComponents[uiWhichComponent].m_bSigned;
    ucBPC = m_akComponents[uiWhichComponent].m_ucBitsPerComponent;
    eRep = m_akComponents[uiWhichComponent].m_eRepresentation;
    eComponent = m_akComponents[uiWhichComponent].m_eComponent;

    if (eComponent != COMP_EMPTY)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
inline void NiPixelFormat::SetComponent(unsigned int uiWhichComponent, 
    NiPixelFormat::Component eComponent, NiPixelFormat::Representation eRep,
    unsigned char ucBPC, bool bSigned)
{
    NIASSERT(uiWhichComponent < NUM_COMPS);
    m_akComponents[uiWhichComponent].m_bSigned = bSigned;
    m_akComponents[uiWhichComponent].m_ucBitsPerComponent = ucBPC;
    m_akComponents[uiWhichComponent].m_eRepresentation = eRep;
    m_akComponents[uiWhichComponent].m_eComponent = eComponent;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelFormat::GetNumComponents() const
{
    for (unsigned int i = 0; i < NUM_COMPS; i++)
    {
        if (m_akComponents[i].m_eComponent == COMP_EMPTY)
            return i;
    }

    return NUM_COMPS;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetPalettized() const
{
    return m_eFormat == FORMAT_PAL || m_eFormat == FORMAT_PALALPHA;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetPaletteHasAlpha() const
{
    return m_eFormat == FORMAT_PALALPHA;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::GetCompressed() const
{
    return ((m_eFormat >= FORMAT_DXT1) && (m_eFormat <= FORMAT_DXT5));
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelFormat::GetMask(Component eComp) const
{
    unsigned int uiShift = 0;
    for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
    {
        if (m_akComponents[ui].m_eComponent == eComp)
        {
            unsigned int uiMask = 0;
            unsigned char ucBPC = m_akComponents[ui].m_ucBitsPerComponent-1;
            if (m_akComponents[ui].m_ucBitsPerComponent == 0)
                return 0;

            while (ucBPC != 0)
            {
                uiMask += 1 << ucBPC;
                ucBPC--;
            }
            uiMask += 1;
            uiMask = uiMask << uiShift;

            if (GetLittleEndian() != 
                NiSystemDesc::GetSystemDesc().IsLittleEndian())
            {
                NiEndian::Swap32((char*)&uiMask);
                uiMask = uiMask >> (32 - m_ucBitsPerPixel);
            }
            return uiMask;
        }
        else
        {
            uiShift += m_akComponents[ui].m_ucBitsPerComponent;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
inline unsigned char NiPixelFormat::GetBits(Component eComp) const
{
    for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
    {
        if (m_akComponents[ui].m_eComponent == eComp)
        {
            return m_akComponents[ui].m_ucBitsPerComponent;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
inline unsigned char NiPixelFormat::GetShift(Component eComp) const
{
    unsigned char ucShift = 0;
    for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
    {
        if (m_akComponents[ui].m_eComponent == eComp)
        {
            if (GetLittleEndian() != 
                NiSystemDesc::GetSystemDesc().IsLittleEndian())
            {
                // For 16 bit endian mismatches, properly compute shifts.
                if (GetBitsPerPixel() == 16)
                {
                    ucShift = ucShift + (ucShift < 8 ? 8 : -8);
                }
                // Original shift computation code (non-16 bit)
                else
                {                    
                    ucShift = ucShift + m_akComponents[ui].m_ucBitsPerComponent;
                    ucShift = m_ucBitsPerPixel - ucShift;
                }

                return ucShift;
            }
            else
            {
                return ucShift;
            }
        }
        else
        {
            ucShift = ucShift + m_akComponents[ui].m_ucBitsPerComponent;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::ComponentCrossesByteBoundary() const
{
    unsigned char ucBits = 0;
    for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
    {      
        ucBits = ucBits + m_akComponents[ui].m_ucBitsPerComponent;
        if (ucBits > 8)
        {
            return true;
        }
        else if (ucBits == 8)
        {
            ucBits -= 8;
        }
    }   

    return false;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::operator==(const NiPixelFormat& kOther) const
{
    if (kOther.m_eFormat != m_eFormat ||
        kOther.m_uFlags != m_uFlags ||
        kOther.m_eTiling != m_eTiling ||
        kOther.m_ucBitsPerPixel != m_ucBitsPerPixel)
    {
        return false;
    }

    if (kOther.m_uiRendererHint != m_uiRendererHint && 
        m_uiRendererHint != INVALID_RENDERER_HINT &&
        kOther.m_uiRendererHint != INVALID_RENDERER_HINT)
    {
        return false;
    }

    for (int i = 0; i < NUM_COMPS; i++)
    {
        if (kOther.m_akComponents[i].m_eComponent != 
            m_akComponents[i].m_eComponent ||
            kOther.m_akComponents[i].m_eRepresentation  != 
            m_akComponents[i].m_eRepresentation ||
            kOther.m_akComponents[i].m_ucBitsPerComponent != 
            m_akComponents[i].m_ucBitsPerComponent ||
            kOther.m_akComponents[i].m_bSigned != 
            m_akComponents[i].m_bSigned)
        {
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
inline bool NiPixelFormat::operator!=(const NiPixelFormat& kOther) const
{
    if (kOther.m_eFormat != m_eFormat ||
        kOther.m_uFlags != m_uFlags ||
        kOther.m_eTiling != m_eTiling ||
        kOther.m_ucBitsPerPixel != m_ucBitsPerPixel)
    {
        return true;
    }

    if (kOther.m_uiRendererHint != m_uiRendererHint && 
        m_uiRendererHint != INVALID_RENDERER_HINT &&
        kOther.m_uiRendererHint != INVALID_RENDERER_HINT)
    {
        return true;
    }

    for (int i = 0; i < NUM_COMPS; i++)
    {
        if (kOther.m_akComponents[i].m_eComponent != 
            m_akComponents[i].m_eComponent ||
            kOther.m_akComponents[i].m_eRepresentation  != 
            m_akComponents[i].m_eRepresentation ||
            kOther.m_akComponents[i].m_ucBitsPerComponent != 
            m_akComponents[i].m_ucBitsPerComponent ||
            kOther.m_akComponents[i].m_bSigned != 
            m_akComponents[i].m_bSigned)
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
inline unsigned char NiPixelFormat::RoundBitsUpToNextByte(
    unsigned char ucNumOfBits) const
{
    return ((ucNumOfBits + 7) & ~7);
}
//---------------------------------------------------------------------------
