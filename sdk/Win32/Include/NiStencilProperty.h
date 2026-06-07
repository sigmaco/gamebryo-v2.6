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

#ifndef NISTENCILPROPERTY_H
#define NISTENCILPROPERTY_H

#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiStencilProperty);

class NIMAIN_ENTRY NiStencilProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiStencilProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    enum TestFunc
    {
        TEST_NEVER, 
        TEST_LESS, 
        TEST_EQUAL, 
        TEST_LESSEQUAL, 
        TEST_GREATER, 
        TEST_NOTEQUAL, 
        TEST_GREATEREQUAL, 
        TEST_ALWAYS, 
        TEST_MAX 
    };

    enum Action
    {
        ACTION_KEEP,
        ACTION_ZERO,
        ACTION_REPLACE,
        ACTION_INCREMENT,
        ACTION_DECREMENT,
        ACTION_INVERT,
        ACTION_MAX
    };

    enum DrawMode
    {
        DRAW_CCW_OR_BOTH,
        DRAW_CCW,
        DRAW_CW,
        DRAW_BOTH,
        DRAW_MAX
    };

    NiStencilProperty();
    ~NiStencilProperty();

    inline void SetStencilOn(bool bOn);
    inline bool GetStencilOn() const;

    inline void SetStencilFunction(TestFunc eFunc);
    TestFunc GetStencilFunction() const;

    inline void SetStencilReference(unsigned int uiRef);
    inline unsigned int GetStencilReference() const;

    inline void SetStencilMask(unsigned int uiMask);
    inline unsigned int GetStencilMask() const;

    inline void SetStencilPassAction(Action eAct);
    Action GetStencilPassAction() const;

    inline void SetStencilPassZFailAction(Action eAct);
    Action GetStencilPassZFailAction() const;

    inline void SetStencilFailAction(Action eAct);
    Action GetStencilFailAction() const;

    inline void SetDrawMode(DrawMode eDraw);
    DrawMode GetDrawMode() const;

    inline bool IsEqualFast(const NiStencilProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiStencilProperty* GetDefault();

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // streaming support
    static char* GetViewerString (const char* pcPrefix, TestFunc eFunc);
    static char* GetViewerString (const char* pcPrefix, Action eAct);
    static char* GetViewerString (const char* pcPrefix, DrawMode eMode);

    // *** end Emergent internal use only ***

protected:

    // flags
    enum
    {
        ENABLE_MASK             = 0x0001,
        FAILACTION_MASK         = 0x000E,
        FAILACTION_POS          = 1,
        ZFAILACTION_MASK        = 0x0070,
        ZFAILACTION_POS         = 4,
        PASSACTION_MASK         = 0x0380,
        PASSACTION_POS          = 7,
        DRAWMODE_MASK           = 0x0C00,
        DRAWMODE_POS            = 10,
        TESTFUNC_MASK           = 0xF000,
        TESTFUNC_POS            = 12
    };

    unsigned int m_uiRef;
    unsigned int m_uiMask;

    static NiStencilPropertyPtr ms_spDefault;
};

typedef NiPointer<NiStencilProperty> NiStencilPropertyPtr;

#include "NiStencilProperty.inl"

#endif

