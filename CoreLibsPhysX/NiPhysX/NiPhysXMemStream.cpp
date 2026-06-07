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

//---------------------------------------------------------------------------
NiPhysXMemStream::NiPhysXMemStream()
{
    m_pkStream = NiNew NiMemStream();
}
//---------------------------------------------------------------------------
NiPhysXMemStream::NiPhysXMemStream(const void* pBuffer, NiUInt32 uiSize)
{
    m_pkStream = NiNew NiMemStream(pBuffer, uiSize);
}
//---------------------------------------------------------------------------
NiPhysXMemStream::~NiPhysXMemStream()
{
    NiDelete m_pkStream;
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXMemStream::GetSize() const
{
    return m_pkStream->GetSize();
}
//---------------------------------------------------------------------------
void* NiPhysXMemStream::GetBuffer()
{
    return m_pkStream->Str();
}
//---------------------------------------------------------------------------
void NiPhysXMemStream::Reset()
{
    m_pkStream->Seek(-(int)m_pkStream->GetSize());
}
//---------------------------------------------------------------------------
NxU8 NiPhysXMemStream::readByte() const
{
    NxU8 kV;
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(&kV, sizeof(NxU8)), sizeof(NxU8));

    return kV;
}
//---------------------------------------------------------------------------
NxU16 NiPhysXMemStream::readWord() const
{
    NxU16 kV;
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(&kV, sizeof(NxU16)), sizeof(NxU16));

    return kV;
}
//---------------------------------------------------------------------------
NxU32 NiPhysXMemStream::readDword() const
{
    NxU32 kV;
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(&kV, sizeof(NxU32)), sizeof(NxU32));

    return kV;
}
//---------------------------------------------------------------------------
NxF32 NiPhysXMemStream::readFloat() const
{
    NxF32 kV;
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(&kV, sizeof(NxF32)), sizeof(NxF32));

    return kV;
}
//---------------------------------------------------------------------------
NxF64 NiPhysXMemStream::readDouble() const
{
    NxF64 kV;
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(&kV, sizeof(NxF64)), sizeof(NxF64));

    return kV;
}
//---------------------------------------------------------------------------
void NiPhysXMemStream::readBuffer(void* buffer, NxU32 size) const
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Read(buffer, size), size);
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeByte(NxU8 b)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(&b, sizeof(NxU8)), sizeof(NxU8));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeWord(NxU16 w)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(&w, sizeof(NxU16)), sizeof(NxU16));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeDword(NxU32 d)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(&d, sizeof(NxU32)), sizeof(NxU32));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeFloat(NxF32 f)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(&f, sizeof(NxF32)), sizeof(NxF32));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeDouble(NxF64 f)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(&f, sizeof(NxF64)), sizeof(NxF64));
    
    return *this;
}
//---------------------------------------------------------------------------
NxStream& NiPhysXMemStream::storeBuffer(const void* buffer, NxU32 size)
{
    NIASSERT(m_pkStream);
    NIVERIFYEQUALS(m_pkStream->Write(buffer, size), size);
    
    return *this;
}
//---------------------------------------------------------------------------
