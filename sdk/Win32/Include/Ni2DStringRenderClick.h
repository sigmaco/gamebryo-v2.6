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

#ifndef NI2DSTRINGRENDERCLICK_H
#define NI2DSTRINGRENDERCLICK_H

#include "NiFontLibType.h"
#include <NiRenderClick.h>
#include "Ni2DString.h"

class NIFONT_ENTRY Ni2DStringRenderClick : public NiRenderClick
{
    NiDeclareRTTI;

public:
    Ni2DStringRenderClick();

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;

    // Functions for accessing Ni2DString objects.
    inline void Append2DString(Ni2DString* pk2DString);
    inline void Prepend2DString(Ni2DString* pk2DString);
    inline void Remove2DString(Ni2DString* pk2DString);
    inline void RemoveAll2DStrings();
    inline NiTPointerList<Ni2DStringPtr>& Get2DStrings();
    inline const NiTPointerList<Ni2DStringPtr>& Get2DStrings() const;

protected:
    // Function for drawing Ni2DString objects.
    virtual void PerformRendering(unsigned int uiFrameID);

    // List of Ni2DString objects.
    NiTPointerList<Ni2DStringPtr> m_k2DStrings;

    // Rendering statistics.
    unsigned int m_uiNumObjectsDrawn;
    float m_fRenderTime;
};

NiSmartPointer(Ni2DStringRenderClick);

#include "Ni2DStringRenderClick.inl"

#endif  // #ifndef NI2DSTRINGRENDERCLICK_H
