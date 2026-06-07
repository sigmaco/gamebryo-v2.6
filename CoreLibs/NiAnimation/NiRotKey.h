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

#ifndef NIROTKEY_H
#define NIROTKEY_H

// The NiRotKey represents animation keys whose values are Gamebryo
// quaternions.  This class serves as a base class for various specialized
// types of keys.  One might consider making NiRotKey a base class with
// virtual function support, but the presence of virtual functions forces
// each object to have a pointer to the virtual function table.  Since an
// animation typically contains a large number of keys, the additional memory
// for the virtual function table pointers can be a burden to the application.
// The virtual functions for the class are "manually" maintained to avoid
// having virtual function table pointers.  The functions themselves are
// stored as an array whose index is one of the NiRotKey::KeyType
// enumerated values.

#include "NiAnimationKey.h"
#include <NiQuaternion.h>

class NIANIMATION_ENTRY NiRotKey : public NiAnimationKey
{
    NiDeclareAnimationStream;
public:
    // attributes
    inline void SetQuaternion (const NiQuaternion& quat);
    inline const NiQuaternion& GetQuaternion () const;

    static NiQuaternion GenInterp (float fTime, NiRotKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize);

    static void GenInterp (float fTime, NiRotKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize, NiQuaternion* pkResult);

    // Function to obtain the coefficients for squad interpolation.
    static void GenCubicCoefs(float fTime, NiRotKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
        unsigned char ucSize, float& fTime0, float& fTime1, 
        NiQuaternion& kQuat0, NiQuaternion& kQuat1, 
        NiQuaternion& kOutQuat0, NiQuaternion& kInQuat1);

    // *** begin Emergent internal use only ***

    inline NiRotKey* GetKeyAt(unsigned int uiIndex, unsigned char ucKeySize);
    static unsigned char GetKeySize(KeyType eType) ;

    static InterpFunction GetInterpFunction (KeyType eType);
    static FillDerivedValsFunction GetFillDerivedFunction (KeyType eType);
    static CubicCoefsFunction GetCubicCoefsFunction(KeyType eType);
    static IsPosedFunction GetIsPosedFunction(KeyType eType);
    static CreateFunction GetCreateFunction (KeyType eType);
    static EqualFunction GetEqualFunction (KeyType eType);
    static CopyFunction GetCopyFunction (KeyType eType);
    static ArrayFunction GetArrayFunction (KeyType eType);
    static DeleteFunction GetDeleteFunction (KeyType eType);
    static InsertFunction GetInsertFunction (KeyType eType);
    static SaveFunction GetSaveFunction (KeyType eType);

    static void SetDefault(const NiQuaternion& kDefault);

    // *** end Emergent internal use only ***

protected:
    NiRotKey ();
    NiRotKey (float fTime, const NiQuaternion& quat);
    NiRotKey (float fTime, float fAngle, const NiPoint3& axis);

    NiQuaternion m_quat;

    static NiQuaternion ms_kDefault;
    static NiQuaternion GenInterpDefault(float fTime, NiRotKey* pkKeys,
        KeyType eType, unsigned int uiNumKeys, unsigned char ucSize);
};

NiRegisterAnimationStream(NiRotKey);

#include "NiRotKey.inl"

#endif
