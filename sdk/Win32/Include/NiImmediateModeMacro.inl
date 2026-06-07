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
inline float NiImmediateModeMacro::GetScreenScaleFactor(
    const NiAVObject* pkObj) const
{
    return GetScreenScaleFactor(pkObj, m_pkAdapter); 
}
//---------------------------------------------------------------------------
inline void NiImmediateModeMacro::ForceAdapterIntoMode(
    NiPrimitiveType::Type eType)
{
    if (m_pkAdapter->IsDrawing() && (m_pkAdapter->GetPrimitiveType() != 
        eType || !m_pkAdapter->IsIndexed()))
    {
        m_pkAdapter->EndDrawing();
        m_pkAdapter->BeginDrawing(eType, true);
    }
    else if (!m_pkAdapter->IsDrawing())
    {
        m_pkAdapter->BeginDrawing(eType, true);
    }
}
//---------------------------------------------------------------------------
