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

#ifndef NIBSPLINEBASIS_H
#define NIBSPLINEBASIS_H

#include "NiRTLib.h"
#include <NiMemObject.h>

template <class REAL, int DEGREE>
class NiBSplineBasis : public NiMemObject
{
public:
    // Open uniform B-spline basis functions.  The input is the number of
    // control points for a B-spline curve using this basis.
    NiBSplineBasis(int iQuantity);
    ~NiBSplineBasis();

    inline int GetQuantity() const;

    // Evaluate the basis functions.  The function GetValue(i) need only be
    // called for iMin <= i <= iMax.  The other values are zero.  This is a
    // consequence of local control.
    inline void Compute(REAL fTime, int& iMin, int& iMax) const;
    inline REAL GetValue(int i) const;

    inline bool IsEqual(const NiBSplineBasis* pkOther) const;

    // *** begin Emergent internal use only ***
    NiBSplineBasis(); // Used for streaming and array allocation
    inline void SaveBinary(NiStream& kStream);
    inline void LoadBinary(NiStream& kStream);
    // *** end Emergent internal use only ***

private:
    // number of control points
    int m_iQuantity;

    // storage for basis evaluation
    mutable REAL m_afValue[DEGREE+1];

    // for avoiding recomputing the basis function
    mutable REAL m_fLastTime;
    mutable int m_iMin, m_iMax;
};

#include "NiBSplineBasis.inl"

#endif
