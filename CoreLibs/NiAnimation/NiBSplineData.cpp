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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiBSplineData.h"
#include <NiCompUtility.h>

NiImplementRTTI(NiBSplineData,NiObject);

//---------------------------------------------------------------------------
NiBSplineData::NiBSplineData() : m_pafControlPoints(NULL), 
    m_pasCompactControlPoints(NULL), 
    m_uiControlPointCount(0), m_uiCompactControlPointCount(0)
{
}
//---------------------------------------------------------------------------
NiBSplineData::~NiBSplineData ()
{
    NiFree(m_pafControlPoints);
    NiFree(m_pasCompactControlPoints);
}
//---------------------------------------------------------------------------
NiBSplineData::Handle 
NiBSplineData::InsertControlPoints(const float* pafControlPoints, 
    unsigned int uiCount, unsigned int uiDimension)
{
    // Determine the new size of the control point array
    unsigned int uiNewSize = m_uiControlPointCount + 
        uiCount * uiDimension;
    float* pafNewCPArray = NiAlloc(float, uiNewSize);

    // Calculate the various array sizes in bytes so that we can
    // memcpy them
    unsigned int uiNewSizeInBytes = uiNewSize * sizeof(float);
    unsigned int uiOldSizeInBytes = m_uiControlPointCount * sizeof(float);

    // Copy the old control point array into the new control point array
    NiMemcpy(pafNewCPArray, uiNewSizeInBytes, m_pafControlPoints, 
        uiOldSizeInBytes);
    
    // Now copy the new control points into the new control point array.
    float* pafNewCPInsert = &pafNewCPArray[m_uiControlPointCount];
    unsigned int uiRemainingSizeInBytes = uiNewSizeInBytes - 
        uiOldSizeInBytes;
    NIASSERT(uiRemainingSizeInBytes == uiCount * uiDimension * sizeof(float));

    NiMemcpy(pafNewCPInsert, uiRemainingSizeInBytes,
        pafControlPoints, uiRemainingSizeInBytes);

    // Reset the internal variables
    m_uiControlPointCount = uiNewSize;
    
    NiFree(m_pafControlPoints);
    m_pafControlPoints = pafNewCPArray;

    // Return the offset of the newly inserted control points
    return (NiBSplineData::Handle) (pafNewCPInsert - pafNewCPArray); 
}
//---------------------------------------------------------------------------
NiBSplineData::Handle 
NiBSplineData::InsertAndCompactControlPoints(const float* pafControlPoints, 
    unsigned int uiCount, unsigned int uiDimension, float& fOffset,
    float& fHalfRange)
{
    // Determine the new size of the control point array
    unsigned int uiNewSize = m_uiCompactControlPointCount + 
        uiCount * uiDimension;
    short* pasNewCPArray = NiAlloc(short,uiNewSize);

    NiCompUtility::NiCompArray kArray = 
        NiCompUtility::CompressFloatArray(pafControlPoints, 
        uiCount*uiDimension);

    // We need to manually delete this
    short* pasCompactControlPoints = kArray.m_asArray; 
    fHalfRange = kArray.m_fHalfRange;
    fOffset = kArray.m_fOffset;

    // Calculate the various array sizes in bytes so that we can
    // memcpy them
    unsigned int uiNewSizeInBytes = uiNewSize * sizeof(short);
    unsigned int uiOldSizeInBytes = m_uiCompactControlPointCount * 
        sizeof(short);

    // Copy the old control point array into the new control point array
    NiMemcpy(pasNewCPArray, uiNewSizeInBytes, m_pasCompactControlPoints, 
        uiOldSizeInBytes);
    
    // Now copy the new control points into the new control point array.
    short* pasNewCPInsert = &pasNewCPArray[m_uiCompactControlPointCount];
    unsigned int uiRemainingSizeInBytes = uiNewSizeInBytes - 
        uiOldSizeInBytes;
    NIASSERT(uiRemainingSizeInBytes == uiCount * uiDimension * sizeof(short));

    NiMemcpy(pasNewCPInsert, uiRemainingSizeInBytes,
        pasCompactControlPoints, uiRemainingSizeInBytes);

    // Reset the internal variables
    m_uiCompactControlPointCount = uiNewSize;
    
    NiFree(m_pasCompactControlPoints);
    m_pasCompactControlPoints = pasNewCPArray;

    NiFree(pasCompactControlPoints);

    // Return the offset of the newly inserted control points
    return (NiBSplineData::Handle ) (pasNewCPInsert - pasNewCPArray); 
}
//---------------------------------------------------------------------------
const float* NiBSplineData::GetControlPoint(Handle kHandle, 
    unsigned int uiIndex, unsigned int uiDimension) const
{
    NIASSERT(kHandle != INVALID_HANDLE);
    return &((m_pafControlPoints + kHandle)[uiIndex * 
        uiDimension]);
}
//---------------------------------------------------------------------------
const short* NiBSplineData::GetCompactControlPoint(Handle kHandle, 
    unsigned int uiIndex, unsigned int uiDimension) const
{
    NIASSERT(kHandle != INVALID_HANDLE);
    return &((m_pasCompactControlPoints + kHandle)[uiIndex * 
        uiDimension]);
}
//---------------------------------------------------------------------------
void NiBSplineData::GetValueDegree3(float fTime, float* afPos,
    unsigned int uiDimension, NiBSplineBasisData* pkBasisData,
    Handle kControlPointHandle) const
{
    NIASSERT(afPos);
    NIASSERT(pkBasisData != NULL);
    NIASSERT(kControlPointHandle != INVALID_HANDLE);

    int iMin, iMax;
    NiBSplineBasis<float, 3>& kBasis = pkBasisData->GetDegree3Basis();
    kBasis.Compute(fTime, iMin, iMax);

    const float* pfSource = GetControlPoint(kControlPointHandle,
        iMin, uiDimension);

    float fBasisValue = kBasis.GetValue(0);
    unsigned int j;
    for (j = 0; j < uiDimension; j++)
        afPos[j] = fBasisValue * (*pfSource++);

    for (int i = iMin + 1, iIndex = 1; i <= iMax; i++, iIndex++)
    {
        fBasisValue = kBasis.GetValue(iIndex);
        for (j = 0; j < uiDimension; j++)
            afPos[j] += fBasisValue * (*pfSource++);
    }
}
//---------------------------------------------------------------------------
void NiBSplineData::GetCompactedValueDegree3(float fTime, float* afPos,
    unsigned int uiDimension, NiBSplineBasisData* pkBasisData, 
    Handle kControlPointHandle, float fOffset,  float fHalfRange) const
{
    NIASSERT(afPos);
    NIASSERT(pkBasisData != NULL);
    NIASSERT(kControlPointHandle != INVALID_HANDLE);

    int iMin, iMax;
    NiBSplineBasis<float, 3>& kBasis = pkBasisData->GetDegree3Basis();
    kBasis.Compute(fTime, iMin, iMax);

    const short* psSource = GetCompactControlPoint(kControlPointHandle,
        iMin, uiDimension);

    unsigned int uiNumItems = uiDimension*4;
    float afSource[16]; // (Degree 3 + 1) * sizeof(NiColorA)/sizeof(float)

    NiCompUtility::DecompressFloatArray(psSource, uiNumItems,
        fOffset, fHalfRange, afSource, uiNumItems);

    float fBasisValue = kBasis.GetValue(0);
    unsigned int srcIdx = 0;
    unsigned int j;
    for (j = 0; j < uiDimension; j++)
        afPos[j] = fBasisValue * afSource[srcIdx++];

    for (int i = iMin + 1, iIndex = 1; i <= iMax; i++, iIndex++)
    {
        fBasisValue = kBasis.GetValue(iIndex);
        for (j = 0; j < uiDimension; j++)
            afPos[j] += fBasisValue * afSource[srcIdx++];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineData);

//---------------------------------------------------------------------------
void NiBSplineData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_uiControlPointCount);
    if (m_uiControlPointCount)
    {
        m_pafControlPoints = NiAlloc(float,m_uiControlPointCount);
        NiStreamLoadBinary(kStream, m_pafControlPoints, 
            m_uiControlPointCount);
    }

    NiStreamLoadBinary(kStream, m_uiCompactControlPointCount);
    if (m_uiCompactControlPointCount)
    {
        m_pasCompactControlPoints = 
            NiAlloc(short,m_uiCompactControlPointCount);
        NiStreamLoadBinary(kStream, m_pasCompactControlPoints, 
            m_uiCompactControlPointCount);
    }
}
//---------------------------------------------------------------------------
void NiBSplineData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiBSplineData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiBSplineData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uiControlPointCount);
    if (m_uiControlPointCount)
    {
        NiStreamSaveBinary(kStream, m_pafControlPoints, 
            m_uiControlPointCount);
    }

    NiStreamSaveBinary(kStream, m_uiCompactControlPointCount);
    if (m_uiCompactControlPointCount)
    {
        NiStreamSaveBinary(kStream, m_pasCompactControlPoints, 
            m_uiCompactControlPointCount);
    }
}
//---------------------------------------------------------------------------
bool NiBSplineData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineData* pkData = (NiBSplineData*) pkObject;
    if (pkData->m_uiControlPointCount != m_uiControlPointCount ||
        pkData->m_uiCompactControlPointCount != m_uiCompactControlPointCount)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBSplineData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBSplineData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiControlPointCount",
        m_uiControlPointCount));

    pStrings->Add(NiGetViewerString("m_uiCompactControlPointCount",
        m_uiCompactControlPointCount));
}
//---------------------------------------------------------------------------

