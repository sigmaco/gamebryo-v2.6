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
#include "NiUISlot.h"
//---------------------------------------------------------------------------
inline
NiUISignal0::NiUISignal0()
{
}
//---------------------------------------------------------------------------
// All code in NiUISignal* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
inline
NiUISignal0::~NiUISignal0()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSlots);
}
//---------------------------------------------------------------------------
inline
void NiUISignal0::EmitSignal() const
{
    NiEmitSignalToAll(m_pkSlots, ());
}
//---------------------------------------------------------------------------
inline
void NiUISignal0::Subscribe(NiUIBaseSlot0* pkSlot)
{
    NiSubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
inline
void NiUISignal0::Unsubscribe(NiUIBaseSlot0* pkSlot)
{
    NiUnsubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
inline
bool NiUISignal0::IsSubscribed(NiUIBaseSlot0* pkSlot) const
{
    return NiIsElementInGroup(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
inline
unsigned int NiUISignal0::NumSubscribed() const
{
    return NiNumElements(m_pkSlots);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
NiUISignal1<Arg1Type>::NiUISignal1()
{
}
//---------------------------------------------------------------------------
// All code in NiUISignal* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
template <typename Arg1Type> inline
NiUISignal1<Arg1Type>::~NiUISignal1()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSlots);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUISignal1<Arg1Type>::EmitSignal(Arg1Type Arg1) const
{
    NiEmitSignalToAll(m_pkSlots, (Arg1));
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUISignal1<Arg1Type>::Subscribe(NiUIBaseSlot1<Arg1Type>* pkSlot)
{
    NiSubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
void NiUISignal1<Arg1Type>::Unsubscribe(NiUIBaseSlot1<Arg1Type>* pkSlot)
{
    NiUnsubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
bool NiUISignal1<Arg1Type>::IsSubscribed(NiUIBaseSlot1<Arg1Type>* pkSlot) const
{
    return NiIsElementInGroup(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type> inline
unsigned int NiUISignal1<Arg1Type>::NumSubscribed() const
{
    return NiNumElements(m_pkSlots);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
NiUISignal2<Arg1Type, Arg2Type>::NiUISignal2()
{
}
//---------------------------------------------------------------------------
// All code in NiUISignal* methods are implemented in the macros defined in
// NiUISignalSlotMacros.h.
template <typename Arg1Type, typename Arg2Type> inline
NiUISignal2<Arg1Type, Arg2Type>::~NiUISignal2()
{
    // Note: None of the pointers in any of these destructors is owned by
    // this instance: Therefore, it should not delete them
    NiUnsubscribeFromAll(m_pkSlots);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUISignal2<Arg1Type, Arg2Type>::EmitSignal(Arg1Type Arg1, Arg2Type Arg2) 
    const
{
    NiEmitSignalToAll(m_pkSlots, (Arg1, Arg2));
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUISignal2<Arg1Type, Arg2Type>::Subscribe(
    NiUIBaseSlot2<Arg1Type, Arg2Type>* pkSlot)
{
    NiSubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
void NiUISignal2<Arg1Type, Arg2Type>::Unsubscribe(
    NiUIBaseSlot2<Arg1Type, Arg2Type>* pkSlot)
{
    NiUnsubscribeToMe(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
bool NiUISignal2<Arg1Type, Arg2Type>::IsSubscribed(
    NiUIBaseSlot2<Arg1Type, Arg2Type>* pkSlot) const
{
    return NiIsElementInGroup(m_pkSlots, pkSlot);
}
//---------------------------------------------------------------------------
template <typename Arg1Type, typename Arg2Type> inline
unsigned int NiUISignal2<Arg1Type, Arg2Type>::NumSubscribed() const
{
    return NiNumElements(m_pkSlots);
}
//---------------------------------------------------------------------------
