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

#ifndef NiTransformData_H
#define NiTransformData_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiRotKey.h"
#include "NiPosKey.h"
#include "NiFloatKey.h"

NiSmartPointer(NiTransformData);

class NIANIMATION_ENTRY NiTransformData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiTransformData ();
    virtual ~NiTransformData ();

    // rotation key access
    inline unsigned int GetNumRotKeys () const;
    inline NiRotKey* GetRotAnim (unsigned int& uiNumKeys,
        NiRotKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping rotation animation sequences at runtime. The 
    //   object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceRotAnim (NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetRotAnim (NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);

    // position key access
    inline unsigned int GetNumPosKeys () const;
    inline NiPosKey* GetPosAnim (unsigned int& uiNumKeys,
        NiPosKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping position animation sequences at runtime. The 
    //   object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplacePosAnim (NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);

    // For swapping position animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetPosAnim (NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);

    // scale key access
    inline unsigned int GetNumScaleKeys () const;
    inline NiFloatKey* GetScaleAnim (unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping scale animation sequences at runtime. The 
    //   object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceScaleAnim (NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // For swapping scale animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetScaleAnim (NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // *** begin Emergent internal use only ***
    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    NiTransformDataPtr GetSequenceData(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    void CleanRotAnim (NiRotKey* pkRotKeys, unsigned int uiRotKeys,
        NiRotKey::KeyType eType);
    
    unsigned int m_uiNumRotKeys;
    NiRotKey* m_pkRotKeys;
    NiRotKey::KeyType m_eRotType;
    unsigned char m_ucRotSize;

    unsigned int m_uiNumPosKeys;
    NiPosKey* m_pkPosKeys;
    NiPosKey::KeyType m_ePosType;
    unsigned char m_ucPosSize;

    unsigned int m_uiNumScaleKeys;
    NiFloatKey* m_pkScaleKeys;
    NiFloatKey::KeyType m_eScaleType;
    unsigned char m_ucScaleSize;
};

#include "NiTransformData.inl"

#endif
