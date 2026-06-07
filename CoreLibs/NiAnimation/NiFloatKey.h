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

#ifndef NIFLOATKEY_H
#define NIFLOATKEY_H

// The NiFloatKey represents animation keys whose values are floating point
// numbers.  This class serves as a base class for various specialized types
// of keys.  One might consider making NiFloatKey a base class with virtual
// function support, but the presence of virtual functions forces each object
// to have a pointer to the virtual function table.  Since an animation
// typically contains a large number of keys, the additional memory for the
// virtual function table pointers can be a burden to the application.  The
// virtual functions for the class are "manually" maintained to avoid having
// virtual function table pointers.  The functions themselves are stored as
// an array whose index is one of the NiFloatKey::KeyType enumerated values.

#include "NiAnimationKey.h"

class NIANIMATION_ENTRY NiFloatKey : public NiAnimationKey
{
    NiDeclareAnimationStream;
public:
    ~NiFloatKey();

    // attributes
    inline void SetValue(float fValue);
    inline float GetValue() const;

    static float GenInterp(float fTime, NiFloatKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize);
    
    // Function to obtain the coefficients for cubic interpolation.
    static void GenCubicCoefs(float fTime, NiFloatKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize, float& fTime0, float& fTime1, 
        float& fValue0, float& fOutTangent0, float& fA0, float& fB0);
    
    // *** begin Emergent internal use only ***

    inline NiFloatKey* GetKeyAt(unsigned int uiIndex, unsigned char ucKeySize);
    static unsigned char GetKeySize(KeyType eType);
    
    static CreateFunction GetCreateFunction(KeyType eType);
    static InterpFunction GetInterpFunction(KeyType eType);
    static EqualFunction GetEqualFunction(KeyType eType);
    static SaveFunction GetSaveFunction(KeyType eType);
    static CopyFunction GetCopyFunction(KeyType eType);
    static ArrayFunction GetArrayFunction(KeyType eType);
    static DeleteFunction GetDeleteFunction(KeyType eType);
    static InsertFunction GetInsertFunction(KeyType eType);
    static FillDerivedValsFunction GetFillDerivedFunction(KeyType eType);
    static CubicCoefsFunction GetCubicCoefsFunction(KeyType eType);
    static IsPosedFunction GetIsPosedFunction(KeyType eType);

    static void SetDefault(float fDefault);

    // *** end Emergent internal use only ***

protected:
    NiFloatKey();
    NiFloatKey(float fTime, float fValue);

    float m_fValue;

    static float ms_fDefault;
    static float GenInterpDefault(float fTime, NiFloatKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned char ucSize);
};

NiRegisterAnimationStream(NiFloatKey);

#include "NiFloatKey.inl"

#endif
