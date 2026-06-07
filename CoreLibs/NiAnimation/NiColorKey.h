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

#ifndef NICOLORKEY_H
#define NICOLORKEY_H

// The NiColorKey represents animation keys whose values are Gamebryo
// NiColorA's.  This class serves as a base class for various specialized
// types of keys.  One might consider making NiColorKey a base class with
// virtual function support, but the presence of virtual functions forces
// each object to have a pointer to the virtual function table.  Since an
// animation typically contains a large number of keys, the additional memory
// for the virtual function table pointers can be a burden to the application.
// The virtual functions for the class are "manually" maintained to avoid
// having virtual function table pointers.  The functions themselves are
// stored as an array whose index is one of the NiColorKey::KeyType
// enumerated values.

#include "NiAnimationKey.h"
#include "NiColor.h"

class NIANIMATION_ENTRY NiColorKey : public NiAnimationKey
{
    NiDeclareAnimationStream;
public:
    ~NiColorKey();

    // attributes
    inline void SetColor(const NiColorA& color);
    inline const NiColorA& GetColor() const;

    static NiColorA GenInterp(float fTime, NiColorKey* pkKeys, KeyType eType, 
        unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize);

    // Function to obtain the coefficients for cubic interpolation.
    static void GenCubicCoefs(float fTime, NiColorKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize, float& fTime0, float& fTime1, 
        NiColorA& kValue0, NiColorA& kOutTangent0,
        NiColorA& kA0, NiColorA& kB0);

    // *** begin Emergent internal use only ***

    inline NiColorKey* GetKeyAt(unsigned int uiIndex, unsigned char ucKeySize);
    static unsigned char GetKeySize(KeyType eType);
    
    // the interpolation "manual" virtual function
    static InterpFunction GetInterpFunction(KeyType eType);
    static CreateFunction GetCreateFunction(KeyType eType);
    static SaveFunction GetSaveFunction(KeyType eType);
    static EqualFunction GetEqualFunction(KeyType eType);
    static CopyFunction GetCopyFunction(KeyType eType);
    static ArrayFunction GetArrayFunction(KeyType eType);
    static DeleteFunction GetDeleteFunction(KeyType eType);
    static InsertFunction GetInsertFunction(KeyType eType);
    static CubicCoefsFunction GetCubicCoefsFunction(KeyType eType);
    static IsPosedFunction GetIsPosedFunction(KeyType eType);

    static void SetDefault(const NiColorA& kDefault);

    // *** end Emergent internal use only ***

protected:
    NiColorKey();
    NiColorKey(float fTime, const NiColorA& col);

    NiColorA m_Color;

    static NiColorA ms_kDefault;
    static NiColorA GenInterpDefault(float fTime, NiColorKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned char ucSize);
};

NiRegisterAnimationStream(NiColorKey);

#include "NiColorKey.inl"

#endif
