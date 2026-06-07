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

#ifndef NITEXTKEY_H
#define NITEXTKEY_H

#include "NiAnimationLibType.h"
#include "NiBool.h"
#include <NiFixedString.h>
class NiStream;

class NIANIMATION_ENTRY NiTextKey : public NiMemObject
{
public:
    // construction and destruction
    NiTextKey ();
    NiTextKey (float fTime, const NiFixedString& kText);
    ~NiTextKey();    

    // attributes
    inline void SetTime (float fTime); 
    inline float GetTime () const;

    void SetText (const NiFixedString& kText);
    inline const NiFixedString& GetText () const;


    // *** begin Emergent internal use only ***

    // copying
    NiTextKey *Copy ();

    // comparison
    inline bool operator== (const NiTextKey& key);
    inline bool operator!= (const NiTextKey& key);

    // streaming
    void LoadBinary (NiStream &stream);
    void SaveBinary (NiStream &stream);
    void RegisterStreamables(NiStream& stream);

    // *** end Emergent internal use only ***

protected:
    float m_fTime;
    NiFixedString m_kText;
};

#include "NiTextKey.inl"

#endif
