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
inline bool NiPhysXMeshDesc::GetIsConvex() const
{
    return GetBit(IS_CONVEX_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::GetIsCloth() const
{
    return GetBit(IS_CLOTH_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::GetIsTriMesh() const
{
    return !GetBit(IS_CLOTH_MASK) && !GetBit(IS_CONVEX_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::GetPlatform(NxPlatform& kPlatform) const
{
    if (GetBit(COOKED_FOR_WIN32_MASK))
    {
        kPlatform = PLATFORM_PC;
        return true;
    }
    else if (GetBit(COOKED_FOR_PS3_MASK))
    {
        kPlatform = PLATFORM_PLAYSTATION3;
        return true;
    }
    else if (GetBit(COOKED_FOR_XENON_MASK))
    {
        kPlatform = PLATFORM_XENON;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetPlatform(const NxPlatform kPlatform)
{
    switch (kPlatform)
    {
        case PLATFORM_PC:
            SetBit(true, COOKED_FOR_WIN32_MASK);
            SetBit(false, COOKED_FOR_XENON_MASK);
            SetBit(false, COOKED_FOR_PS3_MASK);
            break;
        case PLATFORM_PLAYSTATION3:
            SetBit(false, COOKED_FOR_WIN32_MASK);
            SetBit(false, COOKED_FOR_XENON_MASK);
            SetBit(true, COOKED_FOR_PS3_MASK);
            break;
        case PLATFORM_XENON:
            SetBit(false, COOKED_FOR_WIN32_MASK);
            SetBit(true, COOKED_FOR_XENON_MASK);
            SetBit(false, COOKED_FOR_PS3_MASK);
            break;
        default:
            NIASSERT(false && "NiPhysXMeshDesc::SetPlatform unknown platform");
    }
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXMeshDesc::GetFlags() const
{
    return m_uiMeshFlags;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::GetHardware() const
{
    return GetBit(HARDWARE_MASK);
}
//---------------------------------------------------------------------------
inline NxMeshPagingMode NiPhysXMeshDesc::GetMeshPagingMode() const
{
    return m_eMeshPagingMode;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPhysXMeshDesc::GetName() const
{
    return m_kMeshName;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::GetData(size_t& kSize, unsigned char** ppucData)
{
    kSize = m_uiMeshSize;
    (*ppucData) = m_pucMeshData;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::GetVertexMap(NiUInt16& usSize,
    const NiUInt16*& pusMap)
{
    usSize = m_usVertexMapSize;
    pusMap= m_pusVertexMap;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetVertexMap(const NiUInt16 usSize,
    NiUInt16* pusMap)
{
    m_usVertexMapSize = usSize;
    m_pusVertexMap = pusMap;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetIsConvex()
{
    SetBit(true, IS_CONVEX_MASK);
    SetBit(false, IS_CLOTH_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetIsCloth()
{
    SetBit(false, IS_CONVEX_MASK);
    SetBit(true, IS_CLOTH_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetIsTriMesh()
{
    SetBit(false, IS_CONVEX_MASK);
    SetBit(false, IS_CLOTH_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetHardware(const bool bHardware)
{
    SetBit(bHardware, HARDWARE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetFlags(const NxU32 uiFlags)
{
    m_uiMeshFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetMeshPagingMode(const NxMeshPagingMode eMode)
{
    m_eMeshPagingMode = eMode;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetName(const NiFixedString& kName)
{
    m_kMeshName = kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetData(const size_t uiSize,
    unsigned char* pucData)
{
    if (m_pucMeshData)
    {
        NiFree(m_pucMeshData);
    }
    
    m_uiMeshSize = uiSize;
    m_pucMeshData = pucData;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
