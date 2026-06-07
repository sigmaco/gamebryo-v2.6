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
template<class T>
inline NiInterfacePointer<T>::NiInterfacePointer(T* pkInterface)
{
    m_pkInterface = pkInterface;
    if (m_pkInterface)
    {
        m_pkInterface->AddReference();
    }
}
//---------------------------------------------------------------------------
template<class T>
inline NiInterfacePointer<T>::NiInterfacePointer(
    const NiInterfacePointer& kInterfacePtr)
{
    m_pkInterface = kInterfacePtr.m_pkInterface;
    if (m_pkInterface)
    {
        m_pkInterface->AddReference();
    }
}
//---------------------------------------------------------------------------
template<class T>
inline NiInterfacePointer<T>::~NiInterfacePointer()
{
    if (m_pkInterface)
    {
        m_pkInterface->RemoveReference();
    }
}
//---------------------------------------------------------------------------
template<class T>
inline NiInterfacePointer<T>::operator T*() const
{
    return m_pkInterface;
}
//---------------------------------------------------------------------------
template<class T>
inline T& NiInterfacePointer<T>::operator*() const
{
    return *m_pkInterface;
}
//---------------------------------------------------------------------------
template<class T>
inline T* NiInterfacePointer<T>::operator->() const
{
    return m_pkInterface;
}
//---------------------------------------------------------------------------
template<class T>
inline NiInterfacePointer<T>& NiInterfacePointer<T>::operator=(
    const NiInterfacePointer& kInterfacePtr)
{
    if (m_pkInterface != kInterfacePtr.m_pkInterface)
    {
        if (m_pkInterface)
        {
            m_pkInterface->RemoveReference();
        }
        m_pkInterface = kInterfacePtr.m_pkInterface;
        if (m_pkInterface)
        {
            m_pkInterface->AddReference();
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
template<class T>
inline NiInterfacePointer<T>& NiInterfacePointer<T>::operator=(T* pkInterface)
{
    if (m_pkInterface != pkInterface)
    {
        if (m_pkInterface)
        {
            m_pkInterface->RemoveReference();
        }
        m_pkInterface = pkInterface;
        if (m_pkInterface)
        {
            m_pkInterface->AddReference();
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
template<class T>
inline bool NiInterfacePointer<T>::operator==(T* pkInterface) const
{
    return (m_pkInterface == pkInterface);
}
//---------------------------------------------------------------------------
template<class T>
inline bool NiInterfacePointer<T>::operator!=(T* pkInterface) const
{
    return (m_pkInterface != pkInterface);
}
//---------------------------------------------------------------------------
template<class T>
inline bool NiInterfacePointer<T>::operator==(
    const NiInterfacePointer& kInterfacePtr) const
{
    return (m_pkInterface == kInterfacePtr.m_pkInterface);
}
//---------------------------------------------------------------------------
template<class T>
inline bool NiInterfacePointer<T>::operator!=(
    const NiInterfacePointer& kInterfacePtr) const
{
    return (m_pkInterface != kInterfacePtr.m_pkInterface);
}
//---------------------------------------------------------------------------
