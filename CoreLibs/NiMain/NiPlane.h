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

#ifndef NIPLANE_H
#define NIPLANE_H

#ifndef __SPU__
#include "NiTArray.h"
#include "NiStream.h"
#endif

#include "NiPoint3.h"


class NIMAIN_ENTRY NiPlane : public NiMemObject
{
public:
    enum {NO_SIDE, POSITIVE_SIDE, NEGATIVE_SIDE};

    NiPlane();
    NiPlane(const NiPoint3& kNormal, float fConstant);
    NiPlane(const NiPoint3& kNormal, const NiPoint3& kPoint);
    NiPlane(const NiPoint3& kP0, const NiPoint3& kP1, const NiPoint3& kP2);

    inline bool operator==(const NiPlane& kPlane);
    inline bool operator!=(const NiPlane& kPlane);

    inline void SetNormal(const NiPoint3& kNormal);
    inline const NiPoint3& GetNormal() const;
    inline void SetConstant(float fConstant);
    inline float GetConstant() const;

    inline float Distance(const NiPoint3& kPoint) const;
    inline int WhichSide(const NiPoint3& kPoint) const;

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);
    char* GetViewerString(const char* pcPrefix) const;
#endif
    // kPlane is m_kNormal*(x,y,z) = m_fConstant
    NiPoint3 m_kNormal;
    float m_fConstant;

    // *** end Emergent internal use only ***
};

#ifndef __SPU__
typedef NiTPrimitiveArray<NiPlane*> NiPlaneArray;
#endif

//---------------------------------------------------------------------------
//  Inline include
#include "NiPlane.inl"

//---------------------------------------------------------------------------

#endif

