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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiPhysXFileStream::NiPhysXFileStream(const char *pcName, 
    NiFile::OpenMode eMode, NiUInt32 uiBufferSize)
{
    m_pkFile = NiFile::GetFile(pcName, eMode, uiBufferSize);
}
//---------------------------------------------------------------------------
NiPhysXFileStream::~NiPhysXFileStream()
{
    NiDelete m_pkFile;
}
//---------------------------------------------------------------------------
bool NiPhysXFileStream::Ready() const
{
    return m_pkFile != 0;
}
//---------------------------------------------------------------------------
NxU8 NiPhysXFileStream::readByte() const
{
    NxU8 kV;
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(&kV, sizeof(NxU8)), sizeof(NxU8));

    return kV;
}
//---------------------------------------------------------------------------
NxU16 NiPhysXFileStream::readWord() const
{
    NxU16 kV;
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(&kV, sizeof(NxU16)), sizeof(NxU16));

    return kV;
}
//---------------------------------------------------------------------------
NxU32 NiPhysXFileStream::readDword() const
{
    NxU32 kV;
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(&kV, sizeof(NxU32)), sizeof(NxU32));

    return kV;
}
//---------------------------------------------------------------------------
NxF32 NiPhysXFileStream::readFloat() const
{
    NxF32 kV;
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(&kV, sizeof(NxF32)), sizeof(NxF32));

    return kV;
}
//---------------------------------------------------------------------------
NxF64 NiPhysXFileStream::readDouble() const
{
    NxF64 kV;
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(&kV, sizeof(NxF64)), sizeof(NxF64));

    return kV;
}
//---------------------------------------------------------------------------
void NiPhysXFileStream::readBuffer(void* buffer, NxU32 size) const
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Read(buffer, size), size);
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeByte(NxU8 b)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(&b, sizeof(NxU8)), sizeof(NxU8));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeWord(NxU16 w)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(&w, sizeof(NxU16)), sizeof(NxU16));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeDword(NxU32 d)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(&d, sizeof(NxU32)), sizeof(NxU32));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeFloat(NxF32 f)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(&f, sizeof(NxF32)), sizeof(NxF32));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeDouble(NxF64 f)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(&f, sizeof(NxF64)), sizeof(NxF64));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXFileStream::storeBuffer(const void* buffer, NxU32 size)
{
    NIASSERT(m_pkFile != 0);
    NIVERIFYEQUALS(m_pkFile->Write(buffer, size), size);
    
    return *this;
}
//---------------------------------------------------------------------------
