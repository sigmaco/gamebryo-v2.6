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

#include <NiNode.h>

//---------------------------------------------------------------------------
inline NiBoneLODController::NiBoneLODController() : m_iLOD(-1),
    m_uiNumLODs(0)
{
}
//---------------------------------------------------------------------------
inline NiBoneLODController::~NiBoneLODController()
{
    CleanUp();
}
//---------------------------------------------------------------------------
inline bool NiBoneLODController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiNode, m_pkTarget);
}
//---------------------------------------------------------------------------
inline void NiBoneLODController::Update(float)
{
    if (ms_iGlobalLOD != -1 && ms_iGlobalLOD != m_iLOD)
    {
        int iNewLOD = ms_iGlobalLOD;
        if (ms_iGlobalLOD > (int) m_uiNumLODs)
            iNewLOD = m_uiNumLODs - 1;
            
        if (iNewLOD != m_iLOD)
            SetBoneLOD(iNewLOD);
    }
}
//---------------------------------------------------------------------------
inline int NiBoneLODController::GetBoneLOD() const
{
    return m_iLOD;
}
//---------------------------------------------------------------------------
inline unsigned int NiBoneLODController::GetNumberOfBoneLODs() const
{
    return m_uiNumLODs;
}
//---------------------------------------------------------------------------
inline void NiBoneLODController::SetGlobalLOD(int iLevel)
{
    ms_iGlobalLOD = iLevel;
}
//---------------------------------------------------------------------------
inline int NiBoneLODController::GetGlobalLOD() 
{
    return ms_iGlobalLOD;
}
//---------------------------------------------------------------------------
