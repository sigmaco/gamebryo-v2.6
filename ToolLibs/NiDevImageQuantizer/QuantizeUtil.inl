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
inline void RGBAPixel::Set(
    unsigned char ucR, unsigned char ucG,
    unsigned char ucB, unsigned char ucA )
{
    ucRed = ucR; 
    ucGreen = ucG; 
    ucBlue = ucB; 
    ucAlpha = ucA; 
}
//---------------------------------------------------------------------------
inline int QuantizeUtil::GetError()
{
    return m_iErrCode; 
}
//---------------------------------------------------------------------------
