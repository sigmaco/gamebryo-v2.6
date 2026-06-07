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

#ifndef NIPOSKEY_H
#define NIPOSKEY_H

// The NiPosKey represents animation keys whose values are 3D points.  This
// class serves as a base class for various specialized types of keys.  One
// might consider making NiFloatKey a base class with virtual function
// support, but the presence of virtual functions forces each object to have
// a pointer to the virtual function table.  Since an animation typically
// contains a large number of keys, the additional memory for the virtual
// function table pointers can be a burden to the application.  The virtual
// functions for the class are "manually" maintained to avoid having virtual
// function table pointers.  The functions themselves are stored as an array
// whose index is one of the NiPosKey::KeyType enumerated values.

#include <NiPoint3.h>
#include "NiAnimationKey.h"

class NIANIMATION_ENTRY NiPosKey : public NiAnimationKey
{
    NiDeclareAnimationStream;
public:
    ~NiPosKey();

    // attributes
    inline void SetPos(const NiPoint3& pos);
    inline const NiPoint3& GetPos() const;
    
    static NiPoint3 GenInterp(float fTime, NiPosKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize);

    // Function to obtain the coefficients for cubic interpolation.
    static void GenCubicCoefs(float fTime, NiPosKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize, float& fTime0, float& fTime1, 
        NiPoint3& kValue0, NiPoint3& kOutTangent0,
        NiPoint3& kA0, NiPoint3& kB0);

    // *** begin Emergent internal use only ***

    static void CoordinateFrame(float fTime, const NiPosKey* pKey0,
        const NiPosKey* pKey1, KeyType eType, NiPoint3& tangent, 
        NiPoint3& normal, NiPoint3& binormal, float& curvature);

    // maximum curvature calculations
    static void SetMaxCurvatureSampleSize(int iSampleSize);
    static float GetMaxCurvatureSampleDelta();

    inline NiPosKey* GetKeyAt(unsigned int uiIndex, unsigned char ucKeySize);
    static unsigned char GetKeySize(KeyType eType);

    static CreateFunction GetCreateFunction(KeyType eType);
    static CurvatureFunction GetCurvatureFunction(KeyType eType);
    static InterpFunction GetInterpFunction(KeyType eType);
    static InterpFunction GetInterpD1Function(KeyType eType);
    static InterpFunction GetInterpD2Function(KeyType eType);
    static EqualFunction GetEqualFunction(KeyType eType);
    static SaveFunction GetSaveFunction(KeyType eType);
    static CopyFunction GetCopyFunction(KeyType eType);
    static ArrayFunction GetArrayFunction(KeyType eType);
    static DeleteFunction GetDeleteFunction(KeyType eType);
    static InsertFunction GetInsertFunction(KeyType eType);
    static FillDerivedValsFunction GetFillDerivedFunction(KeyType eType);
    static CubicCoefsFunction GetCubicCoefsFunction(KeyType eType);
    static IsPosedFunction GetIsPosedFunction(KeyType eType);

    static void SetDefault(const NiPoint3& kDefault);

    // *** end Emergent internal use only ***

protected:
    NiPosKey();
    NiPosKey(float fTime, const NiPoint3& pos);

    NiPoint3 m_Pos;

    // maximum curvature sample size
    static int ms_iSampleSize;
    static float ms_fSampleDelta;

    static NiPoint3 ms_kDefault;
    static NiPoint3 GenInterpDefault(float fTime, NiPosKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned char ucSize);
};

NiRegisterAnimationStream(NiPosKey);

#include "NiPosKey.inl"

#endif
