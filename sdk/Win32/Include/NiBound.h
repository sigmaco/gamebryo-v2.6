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

#ifndef NIBOUND_H
#define NIBOUND_H

#include "NiPoint3.h"
#include "NiPoint4.h"
#include "NiTransform.h"
#include <NiUniversalTypes.h>

#ifndef __SPU__
#include "NiStream.h"
#include "NiTArray.h"
#else
template <class T> class NiTPrimitiveArray;
#endif

class NiNode;
class NiPlane;


class NIMAIN_ENTRY NiBound : public NiMemObject
{
public:
    // constructor
    NiBound();

    // attributes
    inline void SetCenter(const NiPoint3& kCenter);
    inline void SetCenter(float x, float y, float z);
    inline const NiPoint3& GetCenter() const;
    inline void SetRadius(float fRadius);
    inline float GetRadius() const;
    inline void SetCenterAndRadius(const NiPoint3& kCenter, float fRadius);

    // comparisons
    bool operator==(const NiBound& kBound);
    inline bool operator!=(const NiBound& kBound);
    bool operator<=(const NiBound& kBound);
    bool operator>=(const NiBound& kBound);

    void ComputeFromData(int iQuantity, const void* pkData, 
        const NiUInt32 uiStride);
    void ComputeFromData(int iQuantity, const NiPoint3* pkData);
    void ComputeFromData(int iQuantity, const NiPoint4* pkData);

    inline void Update(const NiBound& kBound, const NiTransform& kXform);

    void ComputeMinimalBound(NiTPrimitiveArray<const NiBound*>& kBound);
    void Merge(const NiBound* pkBound);

    inline int WhichSide(const NiPlane& kPlane) const;

    static bool TestIntersect(float fTime,
        const NiBound& kB0, const NiPoint3& kV0,
        const NiBound& kB1, const NiPoint3& kV1);

    static bool FindIntersect(float fTime,
        const NiBound& kB0, const NiPoint3& kV0,
        const NiBound& kB1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);


    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);
    char* GetViewerString(const char* pcPrefix) const;
#endif
    // *** end Emergent internal use only ***

protected:
    // data specific to bounding spheres
    NiPoint3 m_kCenter;
    float m_fRadius;

private:
    // for bounding sphere containment comparisons
    static float ms_fFuzzFactor;
    static float ms_fTolerance;
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiBound.inl"
//---------------------------------------------------------------------------

#endif

