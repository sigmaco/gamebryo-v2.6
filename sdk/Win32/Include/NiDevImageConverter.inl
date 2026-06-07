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
inline NiDevImageConverter::PixelBits::PixelBits(const NiPixelFormat& kFmt)
{
    if (kFmt.GetFormat() == NiPixelFormat::FORMAT_BUMP || 
        kFmt.GetFormat() == NiPixelFormat::FORMAT_BUMPLUMA)
    {
        m_uiRM = kFmt.GetMask(NiPixelFormat::COMP_OFFSET_U);
        m_ucRS = kFmt.GetShift(NiPixelFormat::COMP_OFFSET_U);
        m_ucRQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_OFFSET_U);
        m_uiGM = kFmt.GetMask(NiPixelFormat::COMP_OFFSET_V);
        m_ucGS = kFmt.GetShift(NiPixelFormat::COMP_OFFSET_V);
        m_ucGQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_OFFSET_V);
        m_uiBM = kFmt.GetMask(NiPixelFormat::COMP_LUMA);
        m_ucBS = kFmt.GetShift(NiPixelFormat::COMP_LUMA);
        m_ucBQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_LUMA);
        m_uiAM = kFmt.GetMask(NiPixelFormat::COMP_HEIGHT);
        m_ucAS = kFmt.GetShift(NiPixelFormat::COMP_HEIGHT);
        m_ucAQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_HEIGHT);
    }
    else
    {
        m_uiRM = kFmt.GetMask(NiPixelFormat::COMP_RED);
        m_ucRS = kFmt.GetShift(NiPixelFormat::COMP_RED);
        m_ucRQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_RED);
        m_uiGM = kFmt.GetMask(NiPixelFormat::COMP_GREEN);
        m_ucGS = kFmt.GetShift(NiPixelFormat::COMP_GREEN);
        m_ucGQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_GREEN);
        m_uiBM = kFmt.GetMask(NiPixelFormat::COMP_BLUE);
        m_ucBS = kFmt.GetShift(NiPixelFormat::COMP_BLUE);
        m_ucBQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_BLUE);
        m_uiAM = kFmt.GetMask(NiPixelFormat::COMP_ALPHA);
        m_ucAS = kFmt.GetShift(NiPixelFormat::COMP_ALPHA);
        m_ucAQ = 8 - kFmt.GetBits(NiPixelFormat::COMP_ALPHA);
    }
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiImageReader*>&
NiDevImageConverter::GetImageReaderList()
{
    return m_kReaders;
}
//---------------------------------------------------------------------------
inline unsigned char NiDevImageConverter::UnpackRedChannel(
    unsigned int uiSrc, NiDevImageConverter::PixelBits& kBits)
{
    // Mask out the non-channel bits and then shift them flush to the LSBs
    unsigned int uiTemp = uiSrc & kBits.m_uiRM;
    uiTemp = uiTemp >> kBits.m_ucRS;

    // Our data is now in the LSBs.  We need to shift such that the MSB of
    // our data is at bit 7.  This will zero out the LSBs unfortunately,
    // so we'll want to or in a mask created from the MSB of the original
    // data.  We have to loop to create this mask.
    unsigned int uiFillBits = 0x00;
    unsigned char ucLoop = kBits.m_ucRQ;
    unsigned char ucQuantum = 8 - kBits.m_ucRQ;
    while (ucQuantum <= ucLoop)
    {
        uiFillBits |= uiTemp;
        uiFillBits = uiFillBits << ucQuantum;
        ucLoop = (unsigned char)(ucLoop - ucQuantum);
    }
    uiFillBits = uiFillBits >> ucQuantum;
    uiFillBits |= uiTemp >> ((8 - kBits.m_ucRQ) - ucLoop);

    // Shift our data back left and return the shifted data OR'd with the
    // fill bits.
    uiTemp = uiTemp << kBits.m_ucRQ;
    return (unsigned char)(uiTemp | uiFillBits);
}
//---------------------------------------------------------------------------
inline unsigned char NiDevImageConverter::UnpackGreenChannel(
    unsigned int uiSrc, NiDevImageConverter::PixelBits& kBits)
{
    // See UnpackRedChannel for description.
    unsigned int uiTemp = uiSrc & kBits.m_uiGM;
    uiTemp = uiTemp >> kBits.m_ucGS;

    unsigned int uiFillBits = 0x00;
    unsigned char ucLoop = kBits.m_ucGQ;
    unsigned char ucQuantum = 8 - kBits.m_ucGQ;
    while (ucQuantum <= ucLoop)
    {
        uiFillBits |= uiTemp;
        uiFillBits = uiFillBits << ucQuantum;
        ucLoop = (unsigned char)(ucLoop - ucQuantum);
    }
    uiFillBits = uiFillBits >> ucQuantum;
    uiFillBits |= uiTemp >> ((8 - kBits.m_ucGQ) - ucLoop);

    uiTemp = uiTemp << kBits.m_ucGQ;
    return (unsigned char)(uiTemp | uiFillBits);

}
//---------------------------------------------------------------------------
inline unsigned char NiDevImageConverter::UnpackBlueChannel(
    unsigned int uiSrc, NiDevImageConverter::PixelBits& kBits)
{
    // See UnpackRedChannel for description.
    unsigned int uiTemp = uiSrc & kBits.m_uiBM;
    uiTemp = uiTemp >> kBits.m_ucBS;

    unsigned int uiFillBits = 0x00;
    unsigned char ucLoop = kBits.m_ucBQ;
    unsigned char ucQuantum = 8 - kBits.m_ucBQ;
    while (ucQuantum <= ucLoop)
    {
        uiFillBits |= uiTemp;
        uiFillBits = uiFillBits << ucQuantum;
        ucLoop = (unsigned char)(ucLoop - ucQuantum);
    }
    uiFillBits = uiFillBits >> ucQuantum;
    uiFillBits |= uiTemp >> ((8 - kBits.m_ucBQ) - ucLoop);

    uiTemp = uiTemp << kBits.m_ucBQ;
    return (unsigned char)(uiTemp | uiFillBits);
}
//---------------------------------------------------------------------------
inline unsigned char NiDevImageConverter::UnpackAlphaChannel(
    unsigned int uiSrc, NiDevImageConverter::PixelBits& kBits)
{
    // See UnpackRedChannel for description.
    unsigned int uiTemp = uiSrc & kBits.m_uiAM;
    uiTemp = uiTemp >> kBits.m_ucAS;

    unsigned int uiFillBits = 0x00;
    unsigned char ucLoop = kBits.m_ucAQ;
    unsigned char ucQuantum = 8 - kBits.m_ucAQ;
    while (ucQuantum <= ucLoop)
    {
        uiFillBits |= uiTemp;
        uiFillBits = uiFillBits << ucQuantum;
        ucLoop = (unsigned char)(ucLoop - ucQuantum);
    }
    uiFillBits = uiFillBits >> ucQuantum;
    uiFillBits |= uiTemp >> ((8 - kBits.m_ucAQ) - ucLoop);

    uiTemp = uiTemp << kBits.m_ucAQ;
    return (unsigned char)(uiTemp | uiFillBits);
}
//---------------------------------------------------------------------------
