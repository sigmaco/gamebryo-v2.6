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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline NiTTerrainRandomAccessIterator<T>::
NiTTerrainRandomAccessIterator() :
    m_pfBuffer(NULL),
    m_uiElementStride(0),
    m_uiTotalStride(0),
    m_bCompressionEnabled(false),
    m_bMorphingEnabled(false)
{
}
//---------------------------------------------------------------------------
template <> inline 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
NiTTerrainRandomAccessIterator(NiDataStreamLock* pkLock, 
    NiUInt32 uiRegionIndex, const NiTerrainConfiguration& kConfiguration)
{
    NiTStridedRandomAccessIterator<float> kBaseIterator = 
        pkLock->begin_region<float>(uiRegionIndex);
    if (kBaseIterator.Exists())
        m_pfBuffer = &kBaseIterator[0];
    else
        m_pfBuffer = NULL;

    m_bCompressionEnabled = false;
    m_bMorphingEnabled = kConfiguration.IsMorphingDataEnabled();
    m_uiElementStride = kConfiguration.GetNumPositionComponents();
    m_uiTotalStride = pkLock->GetDataStream()->GetStride() / sizeof(float);
}
//---------------------------------------------------------------------------
template <> inline 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
NiTTerrainRandomAccessIterator(NiDataStreamLock* pkLock, 
    NiUInt32 uiRegionIndex, const NiTerrainConfiguration& kConfiguration)
{
    NiTStridedRandomAccessIterator<float> kBaseIterator = 
        pkLock->begin_region<float>(uiRegionIndex);
    if (kBaseIterator.Exists())
        m_pfBuffer = &kBaseIterator[0];
    else
        m_pfBuffer = NULL;

    m_bCompressionEnabled = kConfiguration.IsLightingDataCompressionEnabled();
    m_bMorphingEnabled = kConfiguration.IsMorphingDataEnabled();
    m_uiElementStride = kConfiguration.GetNumNormalComponents();
    m_uiTotalStride = pkLock->GetDataStream()->GetStride() / sizeof(float);
}
//---------------------------------------------------------------------------
template <> inline 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
NiTTerrainRandomAccessIterator(NiDataStreamLock* pkLock, 
    NiUInt32 uiRegionIndex, const NiTerrainConfiguration& kConfiguration)
{
    NiTStridedRandomAccessIterator<float> kBaseIterator = 
        pkLock->begin_region<float>(uiRegionIndex);
    if (kBaseIterator.Exists())
        m_pfBuffer = &kBaseIterator[0];
    else
        m_pfBuffer = NULL;

    m_bCompressionEnabled = kConfiguration.IsLightingDataCompressionEnabled();
    m_bMorphingEnabled = kConfiguration.IsMorphingDataEnabled();
    m_uiElementStride = kConfiguration.GetNumTangentComponents();
    m_uiTotalStride = pkLock->GetDataStream()->GetStride() / sizeof(float);
}
//---------------------------------------------------------------------------
template <int T> inline NiTTerrainRandomAccessIterator<T>::
NiTTerrainRandomAccessIterator(NiDataStreamLock* pkLock, 
    NiUInt32 uiRegionIndex, const NiTerrainConfiguration& kConfiguration)
{
    NI_UNUSED_ARG(pkLock);
    NI_UNUSED_ARG(kConfiguration);

    NIASSERT(!"Invalid stream type");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Get(NiUInt32 uiIndex, NiPoint4& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 4);

    kPoint.SetX(m_pfBuffer[uiPos]);
    kPoint.SetY(m_pfBuffer[++uiPos]);
    kPoint.SetZ(m_pfBuffer[++uiPos]);
    kPoint.SetW(m_pfBuffer[++uiPos]);
}
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Get(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 3);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
    kPoint.z = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Get(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 2);
    
    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline float NiTTerrainRandomAccessIterator<T>::
GetComponent(NiUInt32 uiIndex, COMPONENT eComponent) const
{
    NIASSERT((NiUInt32)eComponent < GetElementStride());
    return m_pfBuffer[uiIndex * GetTotalStride() + (NiUInt32)eComponent];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Set(NiUInt32 uiIndex, const NiPoint4& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 4);

    m_pfBuffer[uiPos] = kPoint.X();
    m_pfBuffer[++uiPos] = kPoint.Y();
    m_pfBuffer[++uiPos] = kPoint.Z();
    m_pfBuffer[++uiPos] = kPoint.W();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Set(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 3);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
    m_pfBuffer[++uiPos] = kPoint.z;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
Set(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 2);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
SetComponent(NiUInt32 uiIndex, COMPONENT eComponent, float fValue) const
{
    NIASSERT(GetElementStride() >= (NiUInt32)eComponent);
    m_pfBuffer[uiIndex * GetTotalStride() + (NiUInt32)eComponent] = fValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
GetHighDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 3);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
    kPoint.z = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
GetHighDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();

    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        ((NiPoint4*)&m_pfBuffer[uiPos])->DecompressXYIntoNormalized(
            kPoint);
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = m_pfBuffer[++uiPos];
        kPoint.z = m_pfBuffer[++uiPos];
    }   
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
GetHighDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();

    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = 0.0f;
        kPoint.z = m_pfBuffer[++uiPos];
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = m_pfBuffer[++uiPos];
        kPoint.z = m_pfBuffer[++uiPos];
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
GetLowDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    if (!GetCompressionEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 3);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
    kPoint.z = m_pfBuffer[uiPos + 2];
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
GetLowDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    if (!GetCompressionEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 4);

    ((NiPoint4*)&m_pfBuffer[uiPos])->DecompressZWIntoNormalized(kPoint);
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
GetLowDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const
{
    if (!GetCompressionEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 3);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = 0.0f;
    kPoint.z = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
GetHighDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = m_pfBuffer[++uiPos];
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        NiPoint4 kCompressed;
        kCompressed.CompressNormalizedIntoXY(
            *(NiPoint3*)&m_pfBuffer[uiPos]);
        kPoint.x = kCompressed.X();
        kPoint.y = kCompressed.Y();
    }
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
GetHighDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = m_pfBuffer[++uiPos];
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        kPoint.x = m_pfBuffer[uiPos];
        kPoint.y = m_pfBuffer[uiPos + 2];
    }
}
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
GetHighDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    NI_UNUSED_ARG(uiIndex);
    NI_UNUSED_ARG(kPoint);
    NIASSERT(!"Invalid operation on this stream.");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
GetLowDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    if (!GetCompressionEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 4);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
GetLowDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const
{
    if (!GetCompressionEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 4);

    kPoint.x = m_pfBuffer[uiPos];
    kPoint.y = m_pfBuffer[++uiPos];
}
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
GetLowDetail(NiUInt32 uiIndex,  NiPoint2& kPoint) const
{
    NI_UNUSED_ARG(uiIndex);
    NI_UNUSED_ARG(kPoint);
    NIASSERT(!"Invalid operation on this stream.");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 3);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
    m_pfBuffer[++uiPos] = kPoint.z;
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();

    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        ((NiPoint4*)&m_pfBuffer[uiPos])->CompressNormalizedIntoXY(kPoint);
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = kPoint.y;
        m_pfBuffer[++uiPos] = kPoint.z;
    }
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();

    if (GetCompressionEnabled())
    {
        NIASSERT(GetElementStride() >= 2);

        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = kPoint.z;
    }
    else
    {
        NIASSERT(GetElementStride() >= 3);

        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = 0.0f;
        m_pfBuffer[++uiPos] = kPoint.z;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    if (!GetMorphingEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 4);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
    m_pfBuffer[uiPos + 2] = kPoint.z;
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    if (!GetMorphingEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 4);

    ((NiPoint4*)&m_pfBuffer[uiPos])->CompressNormalizedIntoZW(kPoint);
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const
{
    if (!GetMorphingEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 4);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.z;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    NI_UNUSED_ARG(uiIndex);
    NI_UNUSED_ARG(kPoint);
    NIASSERT(!"Invalid operation on Position stream.");
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 2);

    if (GetCompressionEnabled())
    {
        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = kPoint.y;
    }
    else
    {
        NiPoint4 kCompressed;
        kCompressed.SetX(kPoint.x);
        kCompressed.SetY(kPoint.y);
        kCompressed.DecompressXYIntoNormalized(
            *(NiPoint3*)&m_pfBuffer[uiPos]);
    }
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
SetHighDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    NiUInt32 uiPos = uiIndex * GetTotalStride();
    NIASSERT(GetElementStride() >= 2);

    if (GetCompressionEnabled())
    {
        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = kPoint.y;
    }
    else
    {
        m_pfBuffer[uiPos] = kPoint.x;
        m_pfBuffer[++uiPos] = 0.0f;
        m_pfBuffer[++uiPos] = kPoint.y;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::POSITION>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    NI_UNUSED_ARG(uiIndex);
    NI_UNUSED_ARG(kPoint);
    NIASSERT(!"Invalid operation on Position stream.");
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::NORMAL>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    if (!GetMorphingEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 4);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
}
//---------------------------------------------------------------------------
template <> inline void 
NiTTerrainRandomAccessIterator<NiTTerrainRandomAccessIterator<0>::TANGENT>::
SetLowDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const
{
    if (!GetMorphingEnabled())
        return;

    NiUInt32 uiPos = uiIndex * GetTotalStride() + 2;
    NIASSERT(GetElementStride() >= 4);

    m_pfBuffer[uiPos] = kPoint.x;
    m_pfBuffer[++uiPos] = kPoint.y;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <int T> inline NiUInt32 NiTTerrainRandomAccessIterator<T>::
GetTotalStride() const
{
    return m_uiTotalStride;
}
//---------------------------------------------------------------------------
template <int T> inline NiUInt32 NiTTerrainRandomAccessIterator<T>::
GetElementStride() const
{
    return m_uiElementStride;
}
//---------------------------------------------------------------------------

template <int T> inline bool NiTTerrainRandomAccessIterator<T>::
GetMorphingEnabled() const
{
    return m_bMorphingEnabled;
}
//---------------------------------------------------------------------------
template <int T> inline bool NiTTerrainRandomAccessIterator<T>::
GetCompressionEnabled() const
{
    return m_bCompressionEnabled;
}
//---------------------------------------------------------------------------
template <int T> inline bool NiTTerrainRandomAccessIterator<T>::
Exists() const
{
    return m_pfBuffer != NULL;
}
//---------------------------------------------------------------------------
template <int T> inline void NiTTerrainRandomAccessIterator<T>::
CopyFrom(NiUInt32 uiDestIndex, 
     const NiTTerrainRandomAccessIterator<T>& kSource, 
     NiUInt32 uiSourceIndex) const
{
    NIASSERT(Exists() && kSource.Exists());
    NiUInt32 uiDestOffset = GetTotalStride() * uiDestIndex;
    NiUInt32 uiSourceOffset = kSource.GetTotalStride() * uiSourceIndex;
    NiMemcpy(m_pfBuffer + uiDestOffset, 
        kSource.m_pfBuffer + uiSourceOffset,
        GetElementStride());
}
//---------------------------------------------------------------------------
