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

#ifndef NIBSPLINEFIT_H
#define NIBSPLINEFIT_H

// The least-squares fitting algorithm.  This function selects the control
// points Q[0..m] for the B-spline curve to minimize
//     E(Q) = sum_{k=0}^{n-1} |sum_{j=0}^m B_{i,d}(t[k])*Q[j] - P[k]|^2
// where the input samples are {P[k],t[k]}_{k=0}^{n-1}, with P[k] a
// DIMENSION-tuple and t[k] = k/(n-1) a time, and where m+1 is the desired
// number of control points.  Also, Q = {Q[0],...,Q[m]} and B_{i,d}(t) is the
// B-spline basis function for index i and degree d.

#include "NiBSplineBasis.h"

template <int DIMENSION, int DEGREE>
class NiBSplineFit : public NiMemObject
{
public:
    // Fit a B-spline curve to the input points.  This class does not assume
    // responsibility for deleting the input array.  If it was allocated
    // dynamically, the caller is responsible for deleting it.  The input
    // points are assumed to be uniformly sampled in time.
    NiBSplineFit(int iInQuantity, const float* afInSample, int iOutQuantity);
    ~NiBSplineFit();

    // Member access.
    int GetInQuantity() const;
    const float* GetInSample() const;
    int GetOutQuantity() const;
    const float* GetOutControl() const;
    const NiBSplineBasis<float,DEGREE>* GetBasis() const;

    // The spline is defined for 0 <= t <= 1.  If a t-value is outside [0,1],
    // an open spline clamps t to [0,1].
    void GetPosition(float fTime, float* afPos) const;

    // Error measurements for the fit.  The first function computes the
    // minimum and maximum lengths between the input samples and the B-spline
    // points that correspond to them.  The second function computes the
    // square root of the average of the squared lengths between the input
    // samples and the B-spline points that correspond to them.
    void ExtremeDifferences(float& fMin, float& fMax) const;
    float RootMeanSquaredError() const;

private:
    // Support for solving the normal equations A^T*A*X = A^T*B, where A is
    // a symmetric, banded matrix.
    void CreateBandMatrix(int iSize, int iBands);
    void DestroyBandMatrix();
    void Set(int iRow, int iCol, double dValue);
    double Get(int iRow, int iCol) const;
    void CholeskyFactorBandMatrix();
    bool SolveLower(double* adOutControl) const;
    bool SolveUpper(double* adOutControl) const;

    // The inputs to the constructor, for use in error measurements via
    // ExtremeDifferences and RootMeanSquaredError.
    int m_iInQuantity;
    const float* m_afInSample;

    // The fitted B-spline curve.
    int m_iOutQuantity;
    float* m_afOutControl;
    NiBSplineBasis<float,DEGREE> m_kOutBasis;

    // The symmetric, banded matrix A.
    int m_iSize, m_iBands;
    double** m_aadBand;
};

#include "NiBSplineFit.inl"

#endif
