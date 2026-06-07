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

#ifndef NIBOOLDATA_H
#define NIBOOLDATA_H

#include "NiAnimationLibType.h"
#include "NiObject.h"
#include "NiStream.h"
#include "NiBoolKey.h"

NiSmartPointer(NiBoolData);

class NIANIMATION_ENTRY NiBoolData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBoolData ();
    virtual ~NiBoolData ();

    // visibility key access
    inline unsigned int GetNumKeys () const;
    inline NiBoolKey* GetAnim (unsigned int& uiNumKeys, NiBoolKey::KeyType& eType, 
        unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The NiBoolData object 
    //   assumes ownership of the data array being passed in, and has the 
    //   responsibility for deleting it when finished with it.
    void ReplaceAnim (NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType);
    
    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. 
    void SetAnim (NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType);

    // *** begin Emergent internal use only ***
    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    NiBoolDataPtr GetSequenceData(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiNumKeys;
    NiBoolKey* m_pkKeys;
    NiBoolKey::KeyType m_eType;
    unsigned char m_ucKeySize;
};

#include "NiBoolData.inl"

#endif
