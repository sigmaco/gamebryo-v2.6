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
#include "NiUISignalSlotMacros.h"
#include "NiUISignal.h"
//---------------------------------------------------------------------------
inline
NiUIBaseSlot0::NiUIBaseSlot0()
{
}
//---------------------------------------------------------------------------
// All code in NiUIBaseSlot* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
inline
NiUIBaseSlot0::~NiUIBaseSlot0()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSignals);
}
//---------------------------------------------------------------------------
inline
void NiUIBaseSlot0::Subscribe(NiUISignal0* pkSignal)
{
    NiSubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
inline
void NiUIBaseSlot0::Unsubscribe(NiUISignal0* pkSignal)
{
    NiUnsubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
inline
bool NiUIBaseSlot0::IsSubscribed(NiUISignal0* pkSignal) const
{
    return NiIsElementInGroup(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
inline
unsigned int NiUIBaseSlot0::NumSubscribed() const
{
    return NiNumElements(m_pkSignals);
}
//---------------------------------------------------------------------------
inline
NiUIStaticSlot0::NiUIStaticSlot0() : 
    NiUIBaseSlot0(),
    m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
inline
NiUIStaticSlot0::NiUIStaticSlot0(void (*pfnCallback)()) : 
    NiUIBaseSlot0(),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(pfnCallback != NULL);
}
//---------------------------------------------------------------------------
inline
void NiUIStaticSlot0::Initialize(void (*pfnCallback)()) 
{
    NIASSERT(pfnCallback != NULL);
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline
NiUIStaticSlot0::~NiUIStaticSlot0()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
inline
void NiUIStaticSlot0::ReceiveSignal() const
{
    NIASSERT(m_pfnCallback != NULL);
    (*m_pfnCallback)();
}
//---------------------------------------------------------------------------
template <typename ClassType> inline
NiUIMemberSlot0<ClassType>::NiUIMemberSlot0() :
    NiUIBaseSlot0(),
    m_pkInstance(NULL),
    m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
template <typename ClassType> inline
NiUIMemberSlot0<ClassType>::NiUIMemberSlot0(ClassType* pkInstance, 
    void (ClassType::*pfnCallback)()) :
    NiUIBaseSlot0(),
    m_pkInstance(pkInstance),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(pkInstance != NULL);
    NIASSERT(pfnCallback != NULL);
}
//---------------------------------------------------------------------------
template <typename ClassType> inline
void NiUIMemberSlot0<ClassType>::Initialize(ClassType* pkInstance, 
    void (ClassType::*pfnCallback)()) 
{
    NIASSERT(pkInstance != NULL);
    NIASSERT(pfnCallback != NULL);
    m_pkInstance = pkInstance,
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
template <typename ClassType> inline
NiUIMemberSlot0<ClassType>::~NiUIMemberSlot0()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pkInstance = NULL;
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
template <typename ClassType> inline
void NiUIMemberSlot0<ClassType>::ReceiveSignal() const
{
    NIASSERT(m_pkInstance != NULL);
    NIASSERT(m_pfnCallback != NULL);
    (m_pkInstance->*m_pfnCallback)();
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
NiUIBaseSlot1<Arg1Type>::NiUIBaseSlot1()
{
}
//---------------------------------------------------------------------------
// All code in NiUIBaseSlot* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
template <typename Arg1Type> inline
NiUIBaseSlot1<Arg1Type>::~NiUIBaseSlot1()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSignals);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUIBaseSlot1<Arg1Type>::Subscribe(NiUISignal1<Arg1Type>* pkSignal)
{
    NiSubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUIBaseSlot1<Arg1Type>::Unsubscribe(NiUISignal1<Arg1Type>* pkSignal)
{
    NiUnsubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
bool NiUIBaseSlot1<Arg1Type>::IsSubscribed(NiUISignal1<Arg1Type>* pkSignal) 
    const
{
    return NiIsElementInGroup(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
unsigned int NiUIBaseSlot1<Arg1Type>::NumSubscribed() const
{
    return NiNumElements(m_pkSignals);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
NiUIStaticSlot1<Arg1Type>::NiUIStaticSlot1() :
    NiUIBaseSlot1<Arg1Type>(), m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
NiUIStaticSlot1<Arg1Type>::NiUIStaticSlot1(void (*pfnCallback)(Arg1Type)) :
    NiUIBaseSlot1<Arg1Type>(),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(pfnCallback != NULL);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUIStaticSlot1<Arg1Type>::Initialize(void (*pfnCallback)(Arg1Type)) 
{
    NIASSERT(pfnCallback != NULL);
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
NiUIStaticSlot1<Arg1Type>::~NiUIStaticSlot1()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUIStaticSlot1<Arg1Type>::ReceiveSignal(Arg1Type Arg1) const
{
    NIASSERT(m_pfnCallback != NULL);
    (*m_pfnCallback)(Arg1);
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type> inline
NiUIMemberSlot1<ClassType, Arg1Type>::NiUIMemberSlot1() :
    NiUIBaseSlot1<Arg1Type>(),
    m_pkInstance(NULL),
    m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type> inline
NiUIMemberSlot1<ClassType, Arg1Type>::NiUIMemberSlot1(ClassType* pkInstance, 
    void (ClassType::*pfnCallback)(Arg1Type)) :
    NiUIBaseSlot1<Arg1Type>(),
    m_pkInstance(pkInstance),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(pkInstance != NULL);
    NIASSERT(pfnCallback != NULL);
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type> inline
void NiUIMemberSlot1<ClassType, Arg1Type>::Initialize(ClassType* pkInstance, 
    void (ClassType::*pfnCallback)(Arg1Type)) 
{
    NIASSERT(pkInstance != NULL);
    NIASSERT(pfnCallback != NULL);
    m_pkInstance = pkInstance;
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type> inline
NiUIMemberSlot1<ClassType, Arg1Type>::~NiUIMemberSlot1()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pkInstance = NULL;
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type> inline
void NiUIMemberSlot1<ClassType, Arg1Type>::ReceiveSignal(Arg1Type Arg1) const
{
    NIASSERT(m_pkInstance != NULL);
    NIASSERT(m_pfnCallback != NULL);
    (m_pkInstance->*m_pfnCallback)(Arg1);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
NiUIBaseSlot2<Arg1Type, Arg2Type>::NiUIBaseSlot2()
{
}
//---------------------------------------------------------------------------
// All code in NiUIBaseSlot* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
template <typename Arg1Type, typename Arg2Type> inline
NiUIBaseSlot2<Arg1Type, Arg2Type>::~NiUIBaseSlot2()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSignals);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUIBaseSlot2<Arg1Type, Arg2Type>::Subscribe(
    NiUISignal2<Arg1Type, Arg2Type>* pkSignal)
{
    NiSubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUIBaseSlot2<Arg1Type, Arg2Type>::Unsubscribe(
    NiUISignal2<Arg1Type, Arg2Type>* pkSignal)
{
    NiUnsubscribeToMe(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
bool NiUIBaseSlot2<Arg1Type, Arg2Type>::IsSubscribed(
    NiUISignal2<Arg1Type, Arg2Type>* pkSignal) const
{
    return NiIsElementInGroup(m_pkSignals, pkSignal);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
unsigned int NiUIBaseSlot2<Arg1Type, Arg2Type>::NumSubscribed() const
{
    return NiNumElements(m_pkSignals);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
NiUIStaticSlot2<Arg1Type, Arg2Type>::NiUIStaticSlot2() :
    NiUIBaseSlot2<Arg1Type, Arg2Type>(),
    m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
NiUIStaticSlot2<Arg1Type, Arg2Type>::NiUIStaticSlot2(
    void (*pfnCallback)(Arg1Type, Arg2Type)) :
    NiUIBaseSlot2<Arg1Type, Arg2Type>(),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(pfnCallback != NULL);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUIStaticSlot2<Arg1Type, Arg2Type>::Initialize(
    void (*pfnCallback)(Arg1Type, Arg2Type)) 
{
    NIASSERT(pfnCallback != NULL);
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
NiUIStaticSlot2<Arg1Type, Arg2Type>::~NiUIStaticSlot2()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUIStaticSlot2<Arg1Type, Arg2Type>::ReceiveSignal(Arg1Type Arg1, 
    Arg2Type Arg2) const
{
    NIASSERT(m_pfnCallback != NULL);
    (*m_pfnCallback)(Arg1, Arg2);
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type> inline
NiUIMemberSlot2<ClassType, Arg1Type, Arg2Type>::NiUIMemberSlot2() :
    NiUIBaseSlot2<Arg1Type, Arg2Type>(),
    m_pkInstance(NULL),
    m_pfnCallback(NULL)
{
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type> inline
NiUIMemberSlot2<ClassType, Arg1Type, Arg2Type>::NiUIMemberSlot2(ClassType*
    pkInstance, void (ClassType::*pfnCallback)(Arg1Type, Arg2Type)) :
    NiUIBaseSlot2<Arg1Type, Arg2Type>(),
    m_pkInstance(pkInstance),
    m_pfnCallback(pfnCallback)
{
    NIASSERT(m_pkInstance != NULL);
    NIASSERT(m_pfnCallback != NULL);
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type> inline
void NiUIMemberSlot2<ClassType, Arg1Type, Arg2Type>::Initialize(ClassType*
    pkInstance, void (ClassType::*pfnCallback)(Arg1Type, Arg2Type))
{
    NIASSERT(m_pkInstance != NULL);
    NIASSERT(m_pfnCallback != NULL);
    
    m_pkInstance = pkInstance;
    m_pfnCallback = pfnCallback;
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type> inline
NiUIMemberSlot2<ClassType, Arg1Type, Arg2Type>::~NiUIMemberSlot2()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    m_pkInstance = NULL;
    m_pfnCallback = NULL;
}
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type> inline
void NiUIMemberSlot2<ClassType, Arg1Type, Arg2Type>::ReceiveSignal(
    Arg1Type Arg1, Arg2Type Arg2) const
{
    NIASSERT(m_pkInstance != NULL);
    NIASSERT(m_pfnCallback != NULL);
    (m_pkInstance->*m_pfnCallback)(Arg1, Arg2);
}
//---------------------------------------------------------------------------
