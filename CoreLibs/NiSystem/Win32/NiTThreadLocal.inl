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
template <class T> inline
NiTThreadLocal<T>::NiTThreadLocal()
{
    NIASSERT(sizeof(T) <= sizeof(ThreadLocalReturnType));
    m_kTLSHandle = ::TlsAlloc();
}
//---------------------------------------------------------------------------
template <class T> inline
NiTThreadLocal<T>::NiTThreadLocal(const T& kObject)
{
    NIASSERT(sizeof(T) <= sizeof(ThreadLocalReturnType));
    m_kTLSHandle = ::TlsAlloc();    
    *(this) = kObject;
}
//---------------------------------------------------------------------------
template <class T> inline
NiTThreadLocal<T>::~NiTThreadLocal()
{
    if (IsValid())
        ::TlsFree(m_kTLSHandle);
}
//---------------------------------------------------------------------------
template <class T> inline
bool NiTThreadLocal<T>::IsValid() const
{
    return m_kTLSHandle != TLS_OUT_OF_INDEXES;
}
//---------------------------------------------------------------------------
template <class T> inline
NiTThreadLocal<T>::operator T() const
{
    NIASSERT(IsValid());
    InternalTypeConverter kReturn;
    kReturn.m_kInternal = ::TlsGetValue(m_kTLSHandle);
    return kReturn.m_kTyped;
}
//---------------------------------------------------------------------------
template <class T> inline
NiTThreadLocal<T>& NiTThreadLocal<T>::operator=(const T& kObject)
{
    NIASSERT(IsValid());
    InternalTypeConverter kDest;
    kDest.m_kTyped = kObject;
    ::TlsSetValue(m_kTLSHandle, kDest.m_kInternal);
    return *(this);
}
//---------------------------------------------------------------------------
