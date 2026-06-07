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
inline void NiPortal::SetAdjoiner(NiAVObject* pkAdjoiner)
{ 
    m_pkAdjoiner = pkAdjoiner; 
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPortal::GetAdjoiner() const
{ 
    return m_pkAdjoiner; 
}
//---------------------------------------------------------------------------
inline void NiPortal::SetActive(bool bActive) 
{ 
    SetBit(bActive, ACTIVE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPortal::GetActive() const
{ 
    return GetBit(ACTIVE_MASK); 
}
//---------------------------------------------------------------------------
inline void NiPortal::SetMarked(bool bMarked) 
{ 
    SetBit(bMarked, MARKED_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPortal::GetMarked() const
{ 
    return GetBit(MARKED_MASK); 
}
//---------------------------------------------------------------------------
inline unsigned short NiPortal::GetVertexCount() const
{
    return m_usVertices;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPortal::GetVertices()
{
    return m_pkModelVertex;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiPortal::GetVertices() const
{
    return m_pkModelVertex;
}
//---------------------------------------------------------------------------
inline void NiPortal::SetModelBound(const NiBound& kBound)
{
    m_kModelBound = kBound;
}
//---------------------------------------------------------------------------
inline const NiBound& NiPortal::GetModelBound() const
{
    return m_kModelBound;
}
//---------------------------------------------------------------------------
inline NiBound& NiPortal::GetModelBound()
{
    return m_kModelBound;
}
//---------------------------------------------------------------------------
