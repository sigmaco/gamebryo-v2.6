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

#ifndef NIANIMATIONKEY_H
#define NIANIMATIONKEY_H

#include "NiAnimationLibType.h"
#include "NiAnimationKeyMacros.h"
#include "NiMemObject.h"

class NiStream;

class NIANIMATION_ENTRY NiAnimationKey : public NiMemObject
{
public:
    // "manual" RTTI
    enum KeyContent
    {
        FLOATKEY,
        POSKEY,
        ROTKEY,
        COLORKEY,
        TEXTKEY,
        BOOLKEY,
        NUMKEYCONTENTS
    };

    enum KeyType
    {
        NOINTERP,
        LINKEY,
        BEZKEY,
        TCBKEY,
        EULERKEY,
        STEPKEY,
        NUMKEYTYPES
    };

    NiDeclareAnimationStream;
public:
    ~NiAnimationKey();

    // attributes
    inline void SetTime(float fTime); 
    inline float GetTime() const;
    
    // *** begin Emergent internal use only ***

    inline NiAnimationKey* GetKeyAt(unsigned int uiIndex, 
        unsigned char ucKeySize) const;
    static unsigned char GetKeySize(KeyContent eContent, 
        KeyType eType);

    // manual virtual functions
    typedef float (*CurvatureFunction)(NiAnimationKey* pkKeys,
        unsigned int uiNumKeys);
    static CurvatureFunction GetCurvatureFunction (KeyContent eContent,
        KeyType eType);

    typedef void (*InterpFunction)(float fTime, const NiAnimationKey* pKey0,
        const NiAnimationKey* pKey1, void* pResult);
    static InterpFunction GetInterpFunction(KeyContent eContent, 
        KeyType eType);
    static InterpFunction GetInterpD1Function(KeyContent eContent, 
        KeyType eType);
    static InterpFunction GetInterpD2Function(KeyContent eContent, 
        KeyType eType);

    typedef void (*FillDerivedValsFunction)(NiAnimationKey* pkKeys,
        unsigned int uiNumKeys, unsigned char ucSize);
    static FillDerivedValsFunction GetFillDerivedFunction(KeyContent eContent, 
        KeyType eType);

    typedef void (*CubicCoefsFunction)(const NiAnimationKey* pkKeys, 
        unsigned int uiNumKeys, unsigned int uiIndex, void* pvValue0, 
        void* pvOutTangent0, void* pvA0, void* pvB0);
    static CubicCoefsFunction GetCubicCoefsFunction(KeyContent eContent, 
        KeyType eType);

    typedef bool (*IsPosedFunction)(const NiAnimationKey* pkKeys, 
        unsigned int uiNumKeys);
    static IsPosedFunction GetIsPosedFunction(KeyContent eContent, 
        KeyType eType);

    typedef NiAnimationKey* (*CreateFunction)(NiStream&, unsigned int);
    static CreateFunction GetCreateFunction(KeyContent eContent, 
        KeyType eType);

    typedef void (*SaveFunction)(NiStream& stream, NiAnimationKey* pkKeys,
        unsigned int uiNumKeys);
    static SaveFunction GetSaveFunction(KeyContent eContent, KeyType eType);

    typedef bool (*EqualFunction)(const NiAnimationKey& key0,
        const NiAnimationKey& key1);
    static EqualFunction GetEqualFunction(KeyContent eContent, KeyType eType);

    typedef void (*CopyFunction)(NiAnimationKey* pkNewKey,
        const NiAnimationKey* pkOrigKey);
    static CopyFunction GetCopyFunction(KeyContent eContent, KeyType eType);

    typedef NiAnimationKey* (*ArrayFunction)(unsigned int uiNumKeys);
    static ArrayFunction GetArrayFunction(KeyContent eContent, KeyType eType);

    typedef void (*DeleteFunction)(NiAnimationKey* pkKeyArray);
    static DeleteFunction GetDeleteFunction(KeyContent eContent,
        KeyType eType);

    typedef bool (*InsertFunction)(float fTime, NiAnimationKey*& pkKeys, 
        unsigned int& uiNumKeys);
    static InsertFunction GetInsertFunction(KeyContent eContent,
        KeyType eType);

    // To support NiControllerExtractor.
    // CopySequence copies the keys in the sequence range from ppkOrigKeys
    // to ppkNewKeys.
    static void CopySequence(KeyContent eContent, KeyType eType, 
        const NiAnimationKey* pkOrigKeys, unsigned int uiOrigNumKeys, 
        float fStartTime, float fEndTime, NiAnimationKey*& pkNewKeys,
        unsigned int& uiNewNumKeys);
    static void GuaranteeKeyAtStartAndEnd(KeyContent eContent, KeyType eType, 
        NiAnimationKey*& pkKeys, unsigned int& uiNumKeys, float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    NiAnimationKey();
    NiAnimationKey(float fTime);

    float m_fTime;

    static void SetCreateFunction(KeyContent eContent, KeyType eType, 
        CreateFunction fCreate);
    static void SetCurvatureFunction(KeyContent eContent, KeyType eType, 
        CurvatureFunction fInterp);
    static void SetInterpFunction(KeyContent eContent, KeyType eType, 
        InterpFunction fInterp);
    static void SetInterpD1Function(KeyContent eContent, KeyType eType, 
        InterpFunction fInterp);
    static void SetInterpD2Function(KeyContent eContent, KeyType eType, 
        InterpFunction fInterp);
    static void SetEqualFunction(KeyContent eContent, KeyType eType, 
        EqualFunction fEqual);
    static void SetCopyFunction(KeyContent eContent, KeyType eType, 
        CopyFunction fCopy);
    static void SetArrayFunction(KeyContent eContent, KeyType eType,
        ArrayFunction fArray);
    static void SetDeleteFunction(KeyContent eContent, KeyType eType,
        DeleteFunction fDelete);
    static void SetInsertFunction(KeyContent eContent, KeyType eType, 
        InsertFunction fInsert);
    static void SetSaveFunction(KeyContent eContent, KeyType eType, 
        SaveFunction fSave);
    static void SetFillDerivedValsFunction(KeyContent eContent, KeyType eType, 
        FillDerivedValsFunction fFillDerivedVals);
    static void SetCubicCoefsFunction(KeyContent eContent, KeyType eType, 
        CubicCoefsFunction fCubicCoefs);
    static void SetIsPosedFunction(KeyContent eContent, KeyType eType, 
        IsPosedFunction fIsPosed);

    static void SetKeySize(KeyContent eContent, KeyType eType, 
        unsigned char ucKeySize);

    static bool OkayToInsert(float fTime, const NiAnimationKey* pkKeys,
        unsigned int uiNumKeys, unsigned int& uiInsertAt,
        unsigned char ucSize);


    // "manual" virtual functions
    static CreateFunction ms_loaders[NUMKEYCONTENTS*NUMKEYTYPES];
    static CurvatureFunction ms_curves[NUMKEYCONTENTS*NUMKEYTYPES];
    static InterpFunction ms_interps[NUMKEYCONTENTS*NUMKEYTYPES];
    static InterpFunction ms_interpd1s[NUMKEYCONTENTS*NUMKEYTYPES];
    static InterpFunction ms_interpd2s[NUMKEYCONTENTS*NUMKEYTYPES];
    static EqualFunction ms_equals[NUMKEYCONTENTS*NUMKEYTYPES];
    static CopyFunction ms_copies[NUMKEYCONTENTS*NUMKEYTYPES];
    static ArrayFunction ms_arrays[NUMKEYCONTENTS*NUMKEYTYPES];
    static DeleteFunction ms_deletes[NUMKEYCONTENTS*NUMKEYTYPES];
    static InsertFunction ms_inserts[NUMKEYCONTENTS*NUMKEYTYPES];
    static SaveFunction ms_saves[NUMKEYCONTENTS*NUMKEYTYPES];
    static FillDerivedValsFunction ms_fillderived[NUMKEYCONTENTS*NUMKEYTYPES];
    static CubicCoefsFunction ms_cubiccoefs[NUMKEYCONTENTS*NUMKEYTYPES];
    static IsPosedFunction ms_isposed[NUMKEYCONTENTS*NUMKEYTYPES];

    static unsigned char ms_keysizes[NUMKEYCONTENTS*NUMKEYTYPES];
};

#include "NiAnimationKey.inl"

#endif // NIANIMATIONKEY_H
