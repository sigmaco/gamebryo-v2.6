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
template <class REAL, int DEGREE> inline
NiBSplineBasis<REAL,DEGREE>::NiBSplineBasis(int iQuantity)
{
    NIASSERT(1 <= DEGREE && DEGREE < iQuantity);

    m_iQuantity = iQuantity;
    m_fLastTime = (REAL)-1.0;
    m_iMin = 0;
    m_iMax = DEGREE;
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
NiBSplineBasis<REAL,DEGREE>::NiBSplineBasis()
{
    m_iQuantity = 0;
    m_fLastTime = (REAL)-1.0;
    m_iMin = 0;
    m_iMax = DEGREE;
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
NiBSplineBasis<REAL,DEGREE>::~NiBSplineBasis ()
{
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
int NiBSplineBasis<REAL,DEGREE>::GetQuantity() const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
void NiBSplineBasis<REAL,DEGREE>::Compute(REAL fTime, int& iMin, int& iMax)
    const
{
    NIASSERT((REAL)0.0 <= fTime && fTime <= (REAL)1.0);

    if (fTime == m_fLastTime)
    {
        // The m_afValue[] elements are already correct.
        iMin = m_iMin;
        iMax = m_iMax;
        return;
    }
    m_fLastTime = fTime;

    // Use scaled time and scaled knots so that 1/(Q-D) does not need to
    // be explicitly stored by the class object.  Determine the extreme
    // indices affected by local control.
    REAL fQmD = (REAL)(m_iQuantity - DEGREE);
    REAL fT;
    if (fTime <= (REAL)0.0)
    {
        fT = (REAL)0.0;
        iMin = 0;
        iMax = DEGREE;
    }
    else if (fTime >= (REAL)1.0)
    {
        fT = fQmD;
        iMax = m_iQuantity - 1;
        iMin = iMax - DEGREE;
    }
    else
    {
        fT = fQmD * fTime;
        iMin = (int)fT;
        iMax = iMin + DEGREE;
    }

    // Cache the indices for use by systems sharing the basis object.
    m_iMin = iMin;
    m_iMax = iMax;

    // Precompute knots to eliminate the need for a function GetKnot(...).
    REAL afKnot[2 * DEGREE];
    for (int i0 = 0, i1 = iMax + 1 - DEGREE; i0 < 2 * DEGREE; i0++, i1++)
    {
        if (i1 <= DEGREE)
            afKnot[i0] = (REAL)0.0;
        else if (i1 >= m_iQuantity)
            afKnot[i0] = fQmD;
        else
            afKnot[i0] = (REAL)(i1 - DEGREE);
    }

    // Initialize the basis function evaluation table.  The first DEGREE-1
    // entries are zero, but they do not have to be set explicitly.
    m_afValue[DEGREE] = (REAL)1.0;

    // Update the basis function evaluation table, each iteration overwriting
    // the results from the previous iteration.
    for (int iRow = DEGREE-1; iRow >= 0; iRow--)
    {
        int iK0 = DEGREE, iK1 = iRow;
        REAL fKnot0 = afKnot[iK0], fKnot1 = afKnot[iK1];
        REAL fInvDenom = ((REAL)1.0) / (fKnot0 - fKnot1);
        REAL fC1 = (fKnot0 - fT) * fInvDenom, fC0;
        m_afValue[iRow] = fC1 * m_afValue[iRow+1];

        for (int iCol = iRow+1; iCol < DEGREE; iCol++)
        {
            fC0 = (fT - fKnot1) * fInvDenom;
            m_afValue[iCol] *= fC0;

            fKnot0 = afKnot[++iK0];
            fKnot1 = afKnot[++iK1];
            fInvDenom = ((REAL)1.0) / (fKnot0 - fKnot1);
            fC1 = (fKnot0 - fT) * fInvDenom;
            m_afValue[iCol] += fC1 * m_afValue[iCol+1];
        }

        fC0 = (fT - fKnot1) * fInvDenom;
        m_afValue[DEGREE] *= fC0;
    }
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
REAL NiBSplineBasis<REAL,DEGREE>::GetValue(int i) const
{
    NIASSERT(0 <= i && i <= DEGREE);
    return m_afValue[i];
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
bool NiBSplineBasis<REAL,DEGREE>::IsEqual(const NiBSplineBasis* pkOther) const
{
    NIASSERT(pkOther);
    return m_iQuantity == pkOther->m_iQuantity;
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
void NiBSplineBasis<REAL,DEGREE>::SaveBinary(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, m_iQuantity);
}
//----------------------------------------------------------------------------
template <class REAL, int DEGREE> inline
void NiBSplineBasis<REAL,DEGREE>::LoadBinary(NiStream& kStream)
{
    NiStreamLoadBinary(kStream, m_iQuantity);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Specializations for degree 3.
//----------------------------------------------------------------------------
template<> inline
void NiBSplineBasis<float,3>::Compute(float fTime, int& iMin, int& iMax)
    const
{
    NIASSERT(0.0f <= fTime && fTime <= 1.0f);

    int iQm3 = m_iQuantity - 3;
    int i;

    if (fTime == m_fLastTime)
    {
        // The m_afValue[] elements are already correct.
        iMin = m_iMin;
        iMax = m_iMax;
        return;
    }
    m_fLastTime = fTime;
    
    // Determine the maximum index affected by local control.  Open
    // splines clamp to [0,1].
    if (fTime < 1.0f)
    {
        i = 3 + (int)(fTime * (float)iQm3);
    }
    else // fTime == 1.0f
    {
        i = m_iQuantity - 1;
    }

    m_iMin = iMin = i - 3;
    m_iMax = iMax = i;

    float fQm3 = (float)iQm3;
    float fT = fQm3 * fTime;

    const float fOneThird = 1.0f / 3.0f;

    if (m_iQuantity >= 7)
    {
        int iQm2 = iQm3 + 1, iQm1 = iQm2 + 1;

        float fG0 = (i > 5 ? (float)(i-5) : 0.0f);
        float fG1 = (i > 4 ? (float)(i-4) : 0.0f);
        float fG2 = (float)(i-3);
        float fG3 = (float)(i-2);
        float fG4 = (i < iQm2 ? (float)(i-1) : (float)iQm3);
        float fG5 = (i < iQm3 ? (float)i : (float)iQm3);

        float fInvG3mG1 = (i == 3 ? 1.0f : 0.5f);
        float fInvG4mG2 = (i == iQm1 ? 1.0f : 0.5f);
        float fInvG3mG0 = (i == 3 ? 1.0f : (i == 4 ? 0.5f : fOneThird));
        float fInvG4mG1 = (i == 3 || i == iQm1 ? 0.5f : fOneThird);
        float fInvG5mG2 = (i == iQm1 ? 1.0f :
            (i == iQm2 ? 0.5f : fOneThird));

        float fTmG0 = fT - fG0;
        float fTmG1 = fT - fG1;
        float fTmG2 = fT - fG2;
        float fG3mT = fG3 - fT;
        float fG4mT = fG4 - fT;
        float fG5mT = fG5 - fT;

        float fExpr0 = fG3mT * fInvG3mG1;
        float fExpr1 = fTmG2 * fInvG4mG2;
        float fExpr2 = fInvG3mG0 * fG3mT * fExpr0;
        float fExpr3 = fInvG4mG1 * (fTmG1 * fExpr0 + fG4mT * fExpr1);
        float fExpr4 = fInvG5mG2 * fTmG2 * fExpr1;

        m_afValue[0] = fG3mT * fExpr2;
        m_afValue[1] = fTmG0 * fExpr2 + fG4mT * fExpr3;
        m_afValue[2] = fTmG1 * fExpr3 + fG5mT * fExpr4;
        m_afValue[3] = fTmG2 * fExpr4;
        return;
    }

    if (m_iQuantity == 6)
    {
        if (i == 3)
        {
            float f1mT = 1.0f - fT;
            float f2mT = 2.0f - fT;
            float f3mT = 3.0f - fT;
            float fHalfT = 0.5f * fT;
            float f1mTSqr = f1mT * f1mT;
            float fExpr0 = 0.5f * (fT * f1mT + f2mT * fHalfT);
            float fExpr1 = fOneThird * fT * fHalfT;

            m_afValue[0] = f1mT * f1mTSqr;
            m_afValue[1] = fT * f1mTSqr + f2mT * fExpr0;
            m_afValue[2] = fT * fExpr0 + f3mT * fExpr1;
            m_afValue[3] = fT * fExpr1;
        }
        else if (i == 4)
        {
            float fTm1 = fT - 1.0f;
            float f2mT = 2.0f - fT;
            float f3mT = 3.0f - fT;
            float fHalfT = 0.5f * fT;
            float f1mHalfT = 1.0f - fHalfT;
            float fHalfTm1 = 0.5f * fTm1;
            float f1mHalfTSqr = f1mHalfT * f1mHalfT;
            float fExpr = fOneThird * (fT * f1mHalfT + f3mT * fHalfTm1);
            float fHalfTm1Sqr = fHalfTm1 * fHalfTm1;

            m_afValue[0] = f2mT * f1mHalfTSqr;
            m_afValue[1] = fT * f1mHalfTSqr + f3mT * fExpr;
            m_afValue[2] = fT * fExpr + f3mT * fHalfTm1Sqr;
            m_afValue[3] = fTm1 * fHalfTm1Sqr;
        }
        else  // i == 5
        {
            float fTm1 = fT - 1.0f;
            float fTm2 = fT - 2.0f;
            float f3mT = 3.0f - fT;
            float fHalf3mT = 0.5f * f3mT;
            float fTm2Sqr = fTm2 * fTm2;
            float fExpr0 = fOneThird * f3mT * fHalf3mT;
            float fExpr1 = 0.5f * (fTm1 * fHalf3mT + f3mT * fTm2);

            m_afValue[0] = f3mT * fExpr0;
            m_afValue[1] = fT * fExpr0 + f3mT * fExpr1;
            m_afValue[2] = fTm1 * fExpr1 + f3mT * fTm2Sqr;
            m_afValue[3] = fTm2 * fTm2Sqr;
        }
        return;
    }

    if (m_iQuantity == 5)
    {
        if (i == 3)
        {
            float f1mT = 1.0f - fT;
            float f2mT = 2.0f - fT;
            float fHalfT = 0.5f * fT;
            float f1mTSqr = f1mT * f1mT;
            float fExpr = 0.5f * (fT * f1mT + f2mT * fHalfT);
            float fHalfTSqr = fHalfT * fHalfT;

            m_afValue[0] = f1mT * f1mTSqr;
            m_afValue[1] = fT * f1mTSqr + f2mT * fExpr;
            m_afValue[2] = fT * fExpr + f2mT * fHalfTSqr;
            m_afValue[3] = fT * fHalfTSqr;
        }
        else  // i == 4
        {
            float fTm1 = fT - 1.0f;
            float f2mT = 2.0f - fT;
            float fHalfT = 0.5f * fT;
            float fTm1Sqr = fTm1 * fTm1;
            float f1mHalfT = 1.0f - fHalfT;
            float f1mHalfTSqr = f1mHalfT * f1mHalfT;
            float fExpr = f1mHalfT * (fHalfT + fTm1);

            m_afValue[0] = f2mT * f1mHalfTSqr;
            m_afValue[1] = fT * f1mHalfTSqr + f2mT * fExpr;
            m_afValue[2] = fT * fExpr + f2mT * fTm1Sqr;
            m_afValue[3] = fTm1 * fTm1Sqr;
        }
        return;
    }

    if (m_iQuantity == 4)
    {
        // i == 3
        float f1mT = 1.0f - fT;
        float fTSqr = fT * fT;
        float f1mTSqr = f1mT * f1mT;

        m_afValue[0] = f1mT * f1mTSqr;
        m_afValue[1] = 3.0f * fT * f1mTSqr;
        m_afValue[2] = 3.0f * fTSqr * f1mT;
        m_afValue[3] = fT * fTSqr;
        return;
    }

    // quantity >= 4 is required
    NIASSERT(false);
}
//----------------------------------------------------------------------------
template<> inline
void NiBSplineBasis<double,3>::Compute(double fTime, int& iMin, int& iMax)
    const
{
    NIASSERT(0.0 <= fTime && fTime <= 1.0);

    if (fTime == m_fLastTime)
    {
        // The m_afValue[] elements are already correct.
        return;
    }
    m_fLastTime = fTime;

    int iQm3 = m_iQuantity - 3;
    int i;

    // Determine the maximum index affected by local control.  Open
    // splines clamp to [0,1].
    if (fTime < 1.0)
    {
        i = 3 + (int)(fTime * (double)iQm3);
    }
    else // fTime == 1.0
    {
        i = m_iQuantity - 1;
    }

    iMin = i - 3;
    iMax = i;

    double fQm3 = (double)iQm3;
    double fT = fQm3 * fTime;

    const double fOneThird = 1.0 / 3.0;

    if (m_iQuantity >= 7)
    {
        int iQm2 = iQm3 + 1, iQm1 = iQm2 + 1;

        double fG0 = (i > 5 ? (double)(i-5) : 0.0);
        double fG1 = (i > 4 ? (double)(i-4) : 0.0);
        double fG2 = (double)(i-3);
        double fG3 = (double)(i-2);
        double fG4 = (i < iQm2 ? (double)(i-1) : (double)iQm3);
        double fG5 = (i < iQm3 ? (double)i : (double)iQm3);

        double fInvG3mG1 = (i == 3 ? 1.0 : 0.5);
        double fInvG4mG2 = (i == iQm1 ? 1.0 : 0.5);
        double fInvG3mG0 = (i == 3 ? 1.0 : (i == 4 ? 0.5 : fOneThird));
        double fInvG4mG1 = (i == 3 || i == iQm1 ? 0.5 : fOneThird);
        double fInvG5mG2 = (i == iQm1 ? 1.0 :
            (i == iQm2 ? 0.5 : fOneThird));

        double fTmG0 = fT - fG0;
        double fTmG1 = fT - fG1;
        double fTmG2 = fT - fG2;
        double fG3mT = fG3 - fT;
        double fG4mT = fG4 - fT;
        double fG5mT = fG5 - fT;

        double fExpr0 = fG3mT * fInvG3mG1;
        double fExpr1 = fTmG2 * fInvG4mG2;
        double fExpr2 = fInvG3mG0 * fG3mT * fExpr0;
        double fExpr3 = fInvG4mG1 * (fTmG1 * fExpr0 + fG4mT * fExpr1);
        double fExpr4 = fInvG5mG2 * fTmG2 * fExpr1;

        m_afValue[0] = fG3mT * fExpr2;
        m_afValue[1] = fTmG0 * fExpr2 + fG4mT * fExpr3;
        m_afValue[2] = fTmG1 * fExpr3 + fG5mT * fExpr4;
        m_afValue[3] = fTmG2 * fExpr4;
        return;
    }

    if (m_iQuantity == 6)
    {
        if (i == 3)
        {
            double f1mT = 1.0 - fT;
            double f2mT = 2.0 - fT;
            double f3mT = 3.0 - fT;
            double fHalfT = 0.5 * fT;
            double f1mTSqr = f1mT * f1mT;
            double fExpr0 = 0.5 * (fT * f1mT + f2mT * fHalfT);
            double fExpr1 = fOneThird * fT * fHalfT;

            m_afValue[0] = f1mT * f1mTSqr;
            m_afValue[1] = fT * f1mTSqr + f2mT * fExpr0;
            m_afValue[2] = fT * fExpr0 + f3mT * fExpr1;
            m_afValue[3] = fT * fExpr1;
        }
        else if (i == 4)
        {
            double fTm1 = fT - 1.0;
            double f2mT = 2.0 - fT;
            double f3mT = 3.0 - fT;
            double fHalfT = 0.5 * fT;
            double f1mHalfT = 1.0 - fHalfT;
            double fHalfTm1 = 0.5 * fTm1;
            double f1mHalfTSqr = f1mHalfT * f1mHalfT;
            double fExpr = fOneThird * (fT * f1mHalfT + f3mT * fHalfTm1);
            double fHalfTm1Sqr = fHalfTm1 * fHalfTm1;

            m_afValue[0] = f2mT * f1mHalfTSqr;
            m_afValue[1] = fT * f1mHalfTSqr + f3mT * fExpr;
            m_afValue[2] = fT * fExpr + f3mT * fHalfTm1Sqr;
            m_afValue[3] = fTm1 * fHalfTm1Sqr;
        }
        else  // i == 5
        {
            double fTm1 = fT - 1.0;
            double fTm2 = fT - 2.0;
            double f3mT = 3.0 - fT;
            double fHalf3mT = 0.5 * f3mT;
            double fTm2Sqr = fTm2 * fTm2;
            double fExpr0 = fOneThird * f3mT * fHalf3mT;
            double fExpr1 = 0.5 * (fTm1 * fHalf3mT + f3mT * fTm2);

            m_afValue[0] = f3mT * fExpr0;
            m_afValue[1] = fT * fExpr0 + f3mT * fExpr1;
            m_afValue[2] = fTm1 * fExpr1 + f3mT * fTm2Sqr;
            m_afValue[3] = fTm2 * fTm2Sqr;
        }
        return;
    }

    if (m_iQuantity == 5)
    {
        if (i == 3)
        {
            double f1mT = 1.0 - fT;
            double f2mT = 2.0 - fT;
            double fHalfT = 0.5 * fT;
            double f1mTSqr = f1mT * f1mT;
            double fExpr = 0.5 * (fT * f1mT + f2mT * fHalfT);
            double fHalfTSqr = fHalfT * fHalfT;

            m_afValue[0] = f1mT * f1mTSqr;
            m_afValue[1] = fT * f1mTSqr + f2mT * fExpr;
            m_afValue[2] = fT * fExpr + f2mT * fHalfTSqr;
            m_afValue[3] = fT * fHalfTSqr;
        }
        else  // i == 4
        {
            double fTm1 = fT - 1.0;
            double f2mT = 2.0 - fT;
            double fHalfT = 0.5 * fT;
            double fTm1Sqr = fTm1 * fTm1;
            double f1mHalfT = 1.0 - fHalfT;
            double f1mHalfTSqr = f1mHalfT * f1mHalfT;
            double fExpr = f1mHalfT * (fHalfT + fTm1);

            m_afValue[0] = f2mT * f1mHalfTSqr;
            m_afValue[1] = fT * f1mHalfTSqr + f2mT * fExpr;
            m_afValue[2] = fT * fExpr + f2mT * fTm1Sqr;
            m_afValue[3] = fTm1 * fTm1Sqr;
        }
        return;
    }

    if (m_iQuantity == 4)
    {
        // i == 3
        double f1mT = 1.0 - fT;
        double fTSqr = fT * fT;
        double f1mTSqr = f1mT * f1mT;

        m_afValue[0] = f1mT * f1mTSqr;
        m_afValue[1] = 3.0 * fT * f1mTSqr;
        m_afValue[2] = 3.0 * fTSqr * f1mT;
        m_afValue[3] = fT * fTSqr;
        return;
    }

    // quantity >= 4 is required
    NIASSERT(false);
}
//----------------------------------------------------------------------------
