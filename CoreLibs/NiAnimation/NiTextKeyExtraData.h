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

#ifndef NITEXTKEYEXTRADATA_H
#define NITEXTKEYEXTRADATA_H

#include "NiExtraData.h"
#include "NiTextKey.h"

NiSmartPointer(NiTextKeyExtraData);

class NIANIMATION_ENTRY NiTextKeyExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiTextKeyExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiTextKeyExtraData ();
    NiTextKeyExtraData (NiTextKey* pKeys, unsigned int uiNumKeys);
    virtual ~NiTextKeyExtraData ();

    inline void SetKeys (NiTextKey* pKeys, unsigned int uiNumKeys);
    inline NiTextKey* GetKeys (unsigned int& uiNumKeys) const;

protected:
    unsigned int m_uiNumKeys;
    NiTextKey* m_pKeys;
};

#include "NiTextKeyExtraData.inl"

#endif
