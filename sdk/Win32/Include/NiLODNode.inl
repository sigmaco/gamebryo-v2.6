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
//  NiLODNode inline functions

//---------------------------------------------------------------------------
inline void NiLODNode::SetLODActive(bool bLODActive)
{
    m_bLODActive = bLODActive;
}
//---------------------------------------------------------------------------
inline bool NiLODNode::GetLODActive() const
{
    return m_bLODActive;
}
//---------------------------------------------------------------------------
inline NiLODData* NiLODNode::GetLODData() const
{
    return m_spLODData;
}
//---------------------------------------------------------------------------
inline void NiLODNode::SetLODData(NiLODData* pkLODData)
{
    m_spLODData = pkLODData;
}
//---------------------------------------------------------------------------
inline int NiLODNode::GetGlobalLOD()
{
    return ms_iGlobalLOD;
}
//---------------------------------------------------------------------------
inline void NiLODNode::SetGlobalLOD(int iNewLOD)
{
    ms_iGlobalLOD = iNewLOD;
}
//---------------------------------------------------------------------------
