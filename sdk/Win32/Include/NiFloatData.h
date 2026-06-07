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

#ifndef NIFLOATDATA_H
#define NIFLOATDATA_H

#include "NiAnimationLibType.h"
#include "NiObject.h"
#include "NiFloatKey.h"
#include "NiStream.h"

NiSmartPointer(NiFloatData);

class NIANIMATION_ENTRY NiFloatData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // construction and destruction
    NiFloatData ();
    virtual ~NiFloatData ();

    // member access
    inline unsigned int GetNumKeys () const;
    inline NiFloatKey* GetAnim (unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The object 
    //   assumes ownership of the data array being passed in, and has the 
    //   responsibility for deleting it when finished with it.
    void ReplaceAnim (NiFloatKey* pkFloatKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. 
    void SetAnim (NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // *** begin Emergent internal use only ***
    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    NiFloatDataPtr GetSequenceData(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiNumKeys;
    NiFloatKey* m_pkKeys;
    NiFloatKey::KeyType m_eType;
    unsigned char m_ucKeySize;
};

#include "NiFloatData.inl"

#endif
