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

#ifndef NIZBUFFERPROPERTY_H
#define NIZBUFFERPROPERTY_H

#include "NiProperty.h"
#include "NiFlags.h"

NiSmartPointer(NiZBufferProperty);

class NIMAIN_ENTRY NiZBufferProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiZBufferProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiZBufferProperty();

    enum TestFunction
    {
        TEST_ALWAYS,
        TEST_LESS,
        TEST_EQUAL,
        TEST_LESSEQUAL,
        TEST_GREATER,
        TEST_NOTEQUAL,
        TEST_GREATEREQUAL,
        TEST_NEVER,
      
        TEST_MAX_MODES
    };

    inline void SetZBufferTest(bool bZBufferTest);
    inline bool GetZBufferTest() const;
    inline void SetZBufferWrite(bool bZBufferWrite);
    inline bool GetZBufferWrite() const;
    inline void SetTestFunction(TestFunction eTest);
    TestFunction GetTestFunction() const;

    inline bool IsEqualFast(const NiZBufferProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiZBufferProperty* GetDefault();

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***
    
protected:
    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 2
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        ZBUFFERTEST_MASK    = 0x0001,
        ZBUFFERWRITE_MASK   = 0x0002,
        TESTFUNCTION_MASK   = 0x003C,
        TESTFUNCTION_POS    = 2
    };

    static NiZBufferPropertyPtr ms_spDefault;
};

#include "NiZBufferProperty.inl"

#endif

