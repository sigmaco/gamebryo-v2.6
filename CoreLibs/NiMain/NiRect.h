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

#ifndef NIRECT_H
#define NIRECT_H

#include "NiStream.h"

template <class T> class NiRect  : public NiMemObject
{
public:
    inline NiRect (T left = T(0), T right = T(0), T top = T(0), T bottom = T(0))
    {
        m_left = left;
        m_right = right;
        m_top = top;
        m_bottom = bottom;
    }

    T m_left, m_right, m_top, m_bottom;

    inline T GetWidth() const;
    inline T GetHeight() const;

    // streaming
    inline void LoadBinary (NiStream& stream);
    inline void SaveBinary (NiStream& stream);
    inline char* GetViewerString (const char* pPrefix) const;
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiRect.inl"

//---------------------------------------------------------------------------

#endif

