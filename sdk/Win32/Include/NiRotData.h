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

#ifndef NIROTDATA_H
#define NIROTDATA_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiRotKey.h"

NiSmartPointer(NiRotData);

class NIANIMATION_ENTRY NiRotData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiRotData();
    virtual ~NiRotData();

    // position key access
    inline unsigned int GetNumKeys() const;
    inline NiRotKey* GetAnim(unsigned int& uiNumKeys, NiRotKey::KeyType& eType, 
        unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The object
    // assumes ownership of the data array being passed in, and has the
    // responsibility for deleting it when finished with it.
    void ReplaceAnim (NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. 
    void SetAnim (NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);

    // *** begin Emergent internal use only ***
    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    NiRotDataPtr GetSequenceData(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiNumKeys;
    NiRotKey* m_pkKeys;
    NiRotKey::KeyType m_eType;
    unsigned char m_ucKeySize;
};

#include "NiRotData.inl"

#endif  // #ifndef NIROTDATA_H
