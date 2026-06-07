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

#include "NiFileIStream.h"


//---------------------------------------------------------------------------
NiFileIStream::NiFileIStream(NiFile *pkNiFile) : 
    Imf::IStream("bogus file string")
{
    NIASSERT(pkNiFile != NULL);
    m_pkNiFile = pkNiFile;
}
//---------------------------------------------------------------------------
bool NiFileIStream::read (char c[], int n)
{
    NIASSERT(n>=0);

    unsigned int uiBytesRead = m_pkNiFile->Read(c,(unsigned int)n);

    NIASSERT(uiBytesRead != 0);
    if (uiBytesRead == 0)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
Imf::Int64 NiFileIStream::tellg()
{
    return (Imf::Int64)(m_pkNiFile->ms_iSeekCur);
}
//---------------------------------------------------------------------------
void NiFileIStream::seekg(Imf::Int64 pos)
{
    NIASSERT(pos == (int)pos);

    int iPos = (int)pos;
    m_pkNiFile->Seek(iPos, SEEK_SET);
}
//---------------------------------------------------------------------------
void NiFileIStream::clear()
{
}
//---------------------------------------------------------------------------
