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

//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
NiBSplineFit<DIMENSION,DEGREE>::NiBSplineFit(int iInQuantity,
    const float* afInSample, int iOutQuantity)
    :
    m_iInQuantity(iInQuantity),
    m_afInSample(afInSample),
    m_iOutQuantity(iOutQuantity),
    m_afOutControl(NiAlloc(float,DIMENSION * iOutQuantity)),
    m_kOutBasis(iOutQuantity),
    m_iSize(0),
    m_iBands(0),
    m_aadBand(0)
{
    NIASSERT(1 <= DEGREE && DEGREE < iOutQuantity);
    NIASSERT(iInQuantity >= iOutQuantity);

    double dTMultiplier = 1.0 / (double)(iInQuantity - 1);
    double dTime;
    int i0, i1, i2, iMin, iMax, j;

    // Fit the data points with a B-spline curve using a least-squares error
    // metric.  The problem is of the form A^T*A*X = A^T*B.
    NiBSplineBasis<double,DEGREE> kBasis(iOutQuantity);

    // Construct the matrix A (depends only on the output basis function).
    CreateBandMatrix(iOutQuantity, DEGREE + 1);
    for (i0 = 0; i0 < iOutQuantity; i0++)
    {
        for (i1 = 0; i1 < i0; i1++)
            Set(i0, i1, Get(i1, i0));

        int i1Max = i0 + DEGREE;
        if (i1Max >= iOutQuantity)
            i1Max = iOutQuantity - 1;

        for (i1 = i0; i1 <= i1Max; i1++)
        {
            double dValue = 0.0;
            for (i2 = 0; i2 < iInQuantity; i2++)
            {
                dTime = dTMultiplier * (double)i2;
                kBasis.Compute(dTime, iMin, iMax);
                if (iMin <= i0 && i0 <= iMax && iMin <= i1 && i1 <= iMax)
                {
                    double dB0 = kBasis.GetValue(i0 - iMin);
                    double dB1 = kBasis.GetValue(i1 - iMin);
                    dValue += dB0 * dB1;
                }
            }
            Set(i0, i1, dValue);
        }
    }

    // Construct the matrix B.
    double** aadB = NiAlloc(double*, iOutQuantity);
    int iProduct = iOutQuantity * iInQuantity;
    aadB[0] = NiAlloc(double, iProduct);
    memset(aadB[0], 0, iProduct * sizeof(double));
    for (i0 = 1; i0 < iOutQuantity; i0++)
        aadB[i0] = &aadB[0][iInQuantity * i0];

    for (i0 = 0; i0 < iOutQuantity; i0++)
    {
        for (i1 = 0; i1 < iInQuantity; i1++)
        {
            dTime = dTMultiplier * (double)i1;
            kBasis.Compute(dTime, iMin, iMax);
            if (iMin <= i0 && i0 <= iMax)
                aadB[i0][i1] = kBasis.GetValue(i0 - iMin);
        }
    }

    // Construct the control points for the least-squares curve.
    double* adOutControl = NiAlloc(double, DIMENSION * iOutQuantity);
    memset(adOutControl, 0, DIMENSION * iOutQuantity * sizeof(double));
    double* pdBaseTarget = adOutControl;
    for (i0 = 0; i0 < iOutQuantity; i0++)
    {
        const float* pfSource = afInSample;
        double* adTarget = pdBaseTarget;
        for (i1 = 0; i1 < iInQuantity; i1++)
        {
            double dBValue = aadB[i0][i1];
            for (j = 0; j < DIMENSION; j++)
                adTarget[j] += dBValue * (double)(*pfSource++);
        }
        pdBaseTarget += DIMENSION;
    }

    // Solve A^T*A*OutControl = A^T*B*InSample.
    CholeskyFactorBandMatrix();
    bool bSolved = SolveLower(adOutControl);
    NIASSERT(bSolved);
    bSolved = SolveUpper(adOutControl);
    NIASSERT(bSolved);

    // Set the B-spline control points.
    float* pfTarget = m_afOutControl;
    const double* pdSource = adOutControl;
    for (i0 = 0; i0 < DIMENSION * iOutQuantity; i0++)
        *pfTarget++ = (float)(*pdSource++);

    // Set the first and last output control points to match the first and
    // last input samples.  This supports the typical animation sequence
    // where the first and last pose are important for matching/wrapping.
    float* pfCEnd0 = m_afOutControl;
    const float* pfSEnd0 = m_afInSample;
    float* pfCEnd1 = &m_afOutControl[DIMENSION * (iOutQuantity - 1)];
    const float* pfSEnd1 = &m_afInSample[DIMENSION * (iInQuantity - 1)];
    for (j = 0; j < DIMENSION; j++)
    {
        *pfCEnd0++ = *pfSEnd0++;
        *pfCEnd1++ = *pfSEnd1++;
    }

    NiFree(adOutControl);

    // Delete the matrix B.
    NiFree(aadB[0]);
    NiFree(aadB);

    // Delete the banded matrix A.
    DestroyBandMatrix();
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
NiBSplineFit<DIMENSION,DEGREE>::~NiBSplineFit()
{
    NiFree(m_afOutControl);
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
int NiBSplineFit<DIMENSION,DEGREE>::GetInQuantity() const
{
    return m_iInQuantity;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
const float* NiBSplineFit<DIMENSION,DEGREE>::GetInSample() const
{
    return m_afInSample;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
int NiBSplineFit<DIMENSION,DEGREE>::GetOutQuantity() const
{
    return m_iOutQuantity;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
const float* NiBSplineFit<DIMENSION,DEGREE>::GetOutControl() const
{
    return m_afOutControl;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::GetPosition(float fTime, float* afPos)
    const
{
    NIASSERT(afPos);

    int iMin, iMax;
    m_kOutBasis.Compute(fTime, iMin, iMax);

    float* pfSource = &m_afOutControl[DIMENSION * iMin];
    float fBasisValue = m_kOutBasis.GetValue(0);
    int j;
    for (j = 0; j < DIMENSION; j++)
        afPos[j] = fBasisValue * (*pfSource++);

    for (int i = iMin + 1, iIndex = 1; i <= iMax; i++, iIndex++)
    {
        fBasisValue = m_kOutBasis.GetValue(iIndex);
        for (j = 0; j < DIMENSION; j++)
            afPos[j] += fBasisValue * (*pfSource++);
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
const NiBSplineBasis<float,DEGREE>*
NiBSplineFit<DIMENSION,DEGREE>::GetBasis() const
{
    return &m_kOutBasis;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::ExtremeDifferences(float& fMin,
    float& fMax) const
{
    const float* pfSource = m_afInSample;
    float fTMultiplier = 1.0f / (float)(m_iInQuantity - 1);
    float fTime = 0.0f;
    float afDiff[DIMENSION];
    GetPosition(fTime, afDiff);
    int j;
    for (j = 0; j < DIMENSION; j++)
    {
        afDiff[j] -= *pfSource++;
    }
    float fLength = afDiff[0] * afDiff[0];
    for (j = 1; j < DIMENSION; j++)
    {
        fLength += afDiff[j] * afDiff[j];
    }
    fLength = NiSqrt(fLength);

    fMin = fLength;
    fMax = fLength;
    for (int i = 1; i < m_iInQuantity; i++)
    {
        fTime = fTMultiplier * (float)i;
        GetPosition(fTime, afDiff);
        for (j = 0; j < DIMENSION; j++)
        {
            afDiff[j] -= *pfSource++;
        }
        fLength = afDiff[0] * afDiff[0];
        for (j = 1; j < DIMENSION; j++)
        {
            fLength += afDiff[j] * afDiff[j];
        }
        fLength = NiSqrt(fLength);
        if (fLength > fMax)
            fMax = fLength;
        else if (fLength < fMin)
            fMin = fLength;
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
float NiBSplineFit<DIMENSION,DEGREE>::RootMeanSquaredError() const
{
    const float* pfSource = m_afInSample;
    float fTMultiplier = 1.0f / (float)(m_iInQuantity - 1);
    float fError = 0.0f;
    for (int i = 0; i < m_iInQuantity; i++)
    {
        float fTime = fTMultiplier * (float)i;
        float afDiff[DIMENSION];
        GetPosition(fTime, afDiff);
        int j;
        for (j = 0; j < DIMENSION; j++)
        {
            afDiff[j] -= *pfSource++;
        }
        float fSqrLength = afDiff[0] * afDiff[0];
        for (j = 1; j < DIMENSION; j++)
        {
            fSqrLength += afDiff[j] * afDiff[j];
        }
        fError += fSqrLength;
    }
    fError /= (float)m_iInQuantity;
    fError = NiSqrt(fError);

    return fError;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::CreateBandMatrix(int iSize, int iBands)
{
    NIASSERT(iSize > 0 && iBands > 0);
    m_iSize = iSize;
    m_iBands = iBands;
    m_aadBand = NiAlloc(double*, m_iBands);
    for (int i = 0, iNumItems = m_iSize; i < m_iBands; i++, iNumItems--)
    {
        m_aadBand[i] = NiAlloc(double, iNumItems);
        memset(m_aadBand[i], 0, iNumItems * sizeof(double));
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::DestroyBandMatrix()
{
    for (int i = 0; i < m_iBands; i++)
        NiFree(m_aadBand[i]);

    NiFree(m_aadBand);
    m_aadBand = 0;
    m_iSize = 0;
    m_iBands = 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::Set(int iRow, int iCol, double dValue)
{
    NIASSERT(0 <= iRow && iRow < m_iSize && 0 <= iCol && iCol < m_iSize);

    int iBand = iCol - iRow;
    if (iBand > 0)
    {
        if (iBand < m_iBands && iRow < m_iSize - iBand)
        {
            m_aadBand[iBand][iRow] = dValue;
            return;
        }
    }
    else if (iBand < 0)
    {
        iBand = -iBand;
        if (iBand < m_iBands && iCol < m_iSize - iBand)
        {
            m_aadBand[iBand][iCol] = dValue;
            return;
        }
    }
    else
    {
        m_aadBand[0][iRow] = dValue;
        return;
    }

    // Matrix entry is zero, so no need to set it explicitly.
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
double NiBSplineFit<DIMENSION,DEGREE>::Get(int iRow, int iCol) const
{
    NIASSERT(0 <= iRow && iRow < m_iSize && 0 <= iCol && iCol < m_iSize);

    int iBand = iCol - iRow;
    if (iBand > 0)
    {
        if (iBand < m_iBands && iRow < m_iSize - iBand)
            return m_aadBand[iBand][iRow];
    }
    else if (iBand < 0)
    {
        iBand = -iBand;
        if (iBand < m_iBands && iCol < m_iSize - iBand)
            return m_aadBand[iBand][iCol];
    }
    else
    {
        return m_aadBand[0][iRow];
    }

    return 0.0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
void NiBSplineFit<DIMENSION,DEGREE>::CholeskyFactorBandMatrix()
{
    const int iSizeM1 = m_iSize - 1;
    int k, kMax;
    for (int i = 0; i < m_iSize; i++)
    {
        int jMin = i - m_iBands;
        if (jMin < 0)
            jMin = 0;

        int j;
        for (j = jMin; j < i; j++)
        {
            kMax = j + m_iBands;
            if (kMax > iSizeM1)
                kMax = iSizeM1;

            for (k = i; k <= kMax; k++)
                Set(k, i, Get(k, i) - Get(i, j) * Get(k, j));
        }

        kMax = j + m_iBands;
        if (kMax > iSizeM1)
            kMax = iSizeM1;

        for (k = 0; k < i; k++)
            Set(k, i, Get(i, k));

        double dDiagonal = Get(i, i);
        NIASSERT(dDiagonal > 0.0);
        double dInvSqrt = 1.0 / sqrt(dDiagonal);
        for (k = i; k <= kMax; k++)
            Set(k, i, dInvSqrt * Get(k, i));
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
bool NiBSplineFit<DIMENSION,DEGREE>::SolveLower(double* adOutControl) const
{
    const double dZeroTolerance = 1e-08;

    double* pdBaseTarget = adOutControl;
    for (int iRow = 0; iRow < m_iSize; iRow++)
    {
        if (fabs(Get(iRow, iRow)) < dZeroTolerance)
            return false;

        const double* pdBaseSource = adOutControl;
        double* adTarget = pdBaseTarget;
        int j;
        for (int iCol = 0; iCol < iRow; iCol++)
        {
            const double* pdSource = pdBaseSource;
            double dMatValue = Get(iRow, iCol);
            for (j = 0; j < DIMENSION; j++)
                adTarget[j] -= dMatValue * (*pdSource++);

            pdBaseSource += DIMENSION;
        }

        double dInv = 1.0 / Get(iRow, iRow);
        for (j = 0; j < DIMENSION; j++)
            adTarget[j] *= dInv;

        pdBaseTarget += DIMENSION;
    }

    return true;
}
//----------------------------------------------------------------------------
template <int DIMENSION, int DEGREE>
bool NiBSplineFit<DIMENSION,DEGREE>::SolveUpper(double* adOutControl) const
{
    const double dZeroTolerance = 1e-08;

    double* pdBaseTarget = &adOutControl[DIMENSION * (m_iSize - 1)];
    for (int iRow = m_iSize - 1; iRow >= 0; iRow--)
    {
        if (fabs(Get(iRow, iRow)) < dZeroTolerance)
            return false;

        const double* pdBaseSource = &adOutControl[DIMENSION * (iRow + 1)];
        double* adTarget = pdBaseTarget;
        int j;
        for (int iCol = iRow + 1; iCol < m_iSize; iCol++)
        {
            const double* pdSource = pdBaseSource;
            double dMatValue = Get(iRow, iCol);
            for (j = 0; j < DIMENSION; j++)
                adTarget[j] -= dMatValue * (*pdSource++);

            pdBaseSource += DIMENSION;
        }

        double dInv = 1.0 / Get(iRow, iRow);
        for (j = 0; j < DIMENSION; j++)
            adTarget[j] *= dInv;

        pdBaseTarget -= DIMENSION;
    }

    return true;
}
//----------------------------------------------------------------------------
