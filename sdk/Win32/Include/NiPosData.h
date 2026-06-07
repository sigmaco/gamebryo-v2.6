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

#ifndef NIPOSDATA_H
#define NIPOSDATA_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiPosKey.h"

NiSmartPointer(NiPosData);

class NIANIMATION_ENTRY NiPosData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiPosData ();
    virtual ~NiPosData ();

    // position key access
    inline unsigned int GetNumKeys () const;
    inline NiPosKey* GetAnim (unsigned int& uiNumKeys,
        NiPosKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The object 
    //   assumes ownership of the data array being passed in, and has the 
    //   responsibility for deleting it when finished with it.
    void ReplaceAnim (NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. 
    void SetAnim (NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);

    // *** begin Emergent internal use only ***
    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    NiPosDataPtr GetSequenceData(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiNumKeys;
    NiPosKey* m_pkKeys;
    NiPosKey::KeyType m_eType;
    unsigned char m_ucKeySize;
};

#include "NiPosData.inl"

#endif
