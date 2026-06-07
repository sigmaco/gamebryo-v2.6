//---------------------------------------------------------------------------
// Herein code has been modified by Emergent. The original src for
// code & algorithms comes from the pngquant program. This modification
// seperates the quantization code into a library and removes it's
// integration with the png format. It also simplifies the code a bit,
// adopts hungarian notation, and has moved to c++.
//---------------------------------------------------------------------------
// Copyright (C) 1989, 1991 by Jef Poskanzer.
// Copyright (C) 1997 by Greg Roelofs.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted, provided
// that the above copyright notice appear in all copies and that both that
// copyright notice and this permission notice appear in supporting
// documentation.  This software is provided "as is" without express or
// implied warranty.
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetHistographRxVal( int iIndex )
{
    return m_pHistograph[iIndex].RGBAColor.ucRed * 
        m_pHistograph[iIndex].iValue;
}
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetHistographGxVal( int iIndex )
{
    return m_pHistograph[iIndex].RGBAColor.ucGreen * 
        m_pHistograph[iIndex].iValue;
}
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetHistographBxVal( int iIndex )
{
    return m_pHistograph[iIndex].RGBAColor.ucBlue *
        m_pHistograph[iIndex].iValue;
}
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetHistographAxVal( int iIndex )
{
    return m_pHistograph[iIndex].RGBAColor.ucAlpha * 
        m_pHistograph[iIndex].iValue;
}
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetHistographValue( int iIndex )
{
    return m_pHistograph[iIndex].iValue;
}
//---------------------------------------------------------------------------
inline ColorHistographItem* NiAlphaMapUtility::GetHistograph()
{
    return m_pHistograph;
}
//---------------------------------------------------------------------------
inline void NiAlphaMapUtility::SetHistograph(
    ColorHistographItem* pHistograph )
{
    delete m_pHistograph;
    m_pHistograph = pHistograph;
}
//---------------------------------------------------------------------------
inline ColorHistographItem* NiAlphaMapUtility::GetColorMap()
{
    return m_pColorMap;
}
//---------------------------------------------------------------------------
inline void NiAlphaMapUtility::SetColorMap( ColorHistographItem* pColorMap )
{
    delete m_pColorMap;
    m_pColorMap = pColorMap;
}
//---------------------------------------------------------------------------
inline int NiAlphaMapUtility::GetColorMapA( int iIndex )
{
    if (m_pColorMap == NULL)
        return -1;

    return m_pColorMap[iIndex].RGBAColor.ucAlpha;
}
//---------------------------------------------------------------------------
