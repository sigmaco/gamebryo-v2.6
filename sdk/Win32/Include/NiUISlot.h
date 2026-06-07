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

#ifndef NIUISLOT_H
#define NIUISLOT_H

#include "NiUserInterfaceLibType.h"
#include <NiTSet.h>
//---------------------------------------------------------------------------
// These NiUISignal* classes/templates are defined in NiUISignal.h.  Not 
// included here because that would cause a circular dependancy.
class NiUISignal0;
template <typename Arg1Type>
class NiUISignal1;
template <typename Arg1Type, typename Arg2Type>
class NiUISignal2;
//---------------------------------------------------------------------------
// All zero parameter slots must inherit from NiUIBaseSlot0.  This allows for
// a single signal to emit to slots which call member functions as well as
// non-member or static member functions.
class NiUIBaseSlot0 : public NiMemObject
{
public:
    NiUIBaseSlot0();
    virtual ~NiUIBaseSlot0();
    virtual void ReceiveSignal() const = 0;
    inline void Subscribe(NiUISignal0* pkSignal);
    inline void Unsubscribe(NiUISignal0* pkSignal);
    inline bool IsSubscribed(NiUISignal0* pkSignal) const;
    inline unsigned int NumSubscribed() const;
protected:
    NiTPrimitiveSet <NiUISignal0*> m_pkSignals;
};
//---------------------------------------------------------------------------
// These two classes only add the function callback pointer and the class
// instance pointer (where needed) to the above Base class
class NiUIStaticSlot0 : public NiUIBaseSlot0
{
public:
    NiUIStaticSlot0();
    NiUIStaticSlot0(void (*pfnCallback)());
    inline void Initialize(void (*pfnCallback)());
    virtual ~NiUIStaticSlot0();
    virtual void ReceiveSignal() const;
protected:
    void (*m_pfnCallback)();
};
//---------------------------------------------------------------------------
template <typename ClassType>
class NiUIMemberSlot0 : public NiUIBaseSlot0
{
public:
    NiUIMemberSlot0();
    NiUIMemberSlot0(ClassType* pkInstance, void (ClassType::*pfnCallback)());
    inline void Initialize(ClassType* pkInstance, void (ClassType::*pfnCallback)());
    virtual ~NiUIMemberSlot0();
    virtual void ReceiveSignal() const;
protected:
    ClassType* m_pkInstance;
    void (ClassType::*m_pfnCallback)();
};
//---------------------------------------------------------------------------
// All one parameter slots must inherit from NiUIBaseSlot1.  This allows for
// a single signal to emit to slots which call member functions as well as
// non-member or static member functions.
template <typename Arg1Type>
class NiUIBaseSlot1 : public NiMemObject
{
public:
    NiUIBaseSlot1();
    virtual ~NiUIBaseSlot1();
    virtual void ReceiveSignal(Arg1Type Arg1) const = 0;
    inline void Subscribe(NiUISignal1<Arg1Type>* pkSignal);
    inline void Unsubscribe(NiUISignal1<Arg1Type>* pkSignal);
    inline bool IsSubscribed(NiUISignal1<Arg1Type>* pkSignal) const;
    inline unsigned int NumSubscribed() const;
protected:
    NiTPrimitiveSet <NiUISignal1<Arg1Type>*> m_pkSignals;
};
//---------------------------------------------------------------------------
// These two classes only add the function callback pointer and the class
// instance pointer (where needed) to the above Base class
template <typename Arg1Type>
class NiUIStaticSlot1 : public NiUIBaseSlot1<Arg1Type>
{
public:
    NiUIStaticSlot1();
    NiUIStaticSlot1(void (*pfnCallback)(Arg1Type));
    inline void Initialize(void (*pfnCallback)(Arg1Type));
    virtual ~NiUIStaticSlot1();
    virtual void ReceiveSignal(Arg1Type Arg1) const;
protected:
    void (*m_pfnCallback)(Arg1Type);
};
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type>
class NiUIMemberSlot1 : public NiUIBaseSlot1<Arg1Type>
{
public:
    NiUIMemberSlot1();
    NiUIMemberSlot1(ClassType* pkInstance, 
        void (ClassType::*pfnCallback)(Arg1Type));
    inline void Initialize(ClassType* pkInstance, 
        void (ClassType::*pfnCallback)(Arg1Type));
    virtual ~NiUIMemberSlot1();
    virtual void ReceiveSignal(Arg1Type Arg1) const;
protected:
    ClassType* m_pkInstance;
    void (ClassType::*m_pfnCallback)(Arg1Type);
};
//---------------------------------------------------------------------------
// All two parameter slots must inherit from NiUIBaseSlot2.  This allows for
// a single signal to emit to slots which call member functions as well as
// non-member or static member functions.
template <typename Arg1Type, typename Arg2Type>
class NiUIBaseSlot2 : public NiMemObject
{
public:
    NiUIBaseSlot2();
    virtual ~NiUIBaseSlot2();
    virtual void ReceiveSignal(Arg1Type Arg1, Arg2Type Arg2) const = 0;
    inline void Subscribe(NiUISignal2<Arg1Type, Arg2Type>* pkSignal);
    inline void Unsubscribe(NiUISignal2<Arg1Type, Arg2Type>* pkSignal);
    inline bool IsSubscribed(NiUISignal2<Arg1Type, Arg2Type>* pkSignal) const;
    inline unsigned int NumSubscribed() const;
protected:
    NiTPrimitiveSet <NiUISignal2<Arg1Type, Arg2Type>*> m_pkSignals;
};
//---------------------------------------------------------------------------
// These two classes only add the function callback pointer and the class
// instance pointer (where needed) to the above Base class
template <typename Arg1Type, typename Arg2Type>
class NiUIStaticSlot2 : 
    public NiUIBaseSlot2<Arg1Type, Arg2Type>
{
public:
    NiUIStaticSlot2();
    NiUIStaticSlot2(void (*pfnCallback)(Arg1Type, Arg2Type));
    inline void Initialize(void (*pfnCallback)(Arg1Type, Arg2Type));
    virtual ~NiUIStaticSlot2();
    virtual void ReceiveSignal(Arg1Type Arg1, Arg2Type Arg2) const;
protected:
    void (*m_pfnCallback)(Arg1Type, Arg2Type);
};
//---------------------------------------------------------------------------
template <typename ClassType, typename Arg1Type, typename Arg2Type>
class NiUIMemberSlot2 : 
    public NiUIBaseSlot2<Arg1Type, Arg2Type>
{
public:
    NiUIMemberSlot2();
    NiUIMemberSlot2(ClassType* pkInstance, 
        void (ClassType::*pfnCallback)(Arg1Type, Arg2Type));
    inline void Initialize(ClassType* pkInstance, 
        void (ClassType::*pfnCallback)(Arg1Type, Arg2Type));
    virtual ~NiUIMemberSlot2();
    virtual void ReceiveSignal(Arg1Type Arg1, Arg2Type Arg2) const;
protected:
    ClassType* m_pkInstance;
    void (ClassType::*m_pfnCallback)(Arg1Type, Arg2Type);
};
//---------------------------------------------------------------------------

#include "NiUISlot.inl"

#endif // NIUISLOT_H

