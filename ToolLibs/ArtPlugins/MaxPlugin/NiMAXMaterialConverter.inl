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
//  NiMAXMaterialConverter inline

//---------------------------------------------------------------------------
inline NiMAXMaterialConverter::NiMAXMaterialConverter(TimeValue animStart,
                                                      TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}

//---------------------------------------------------------------------------
inline bool NiMAXMaterialConverter::IsSceneTrans()
{
    return ms_bSceneIsTrans;
}
//---------------------------------------------------------------------------

inline void NiMAXMaterialConverter::Init()
{
    NIASSERT(!ms_bSceneIsTrans);
    
}
//---------------------------------------------------------------------------
inline void NiMAXMaterialConverter::Shutdown()
{
    ms_bSceneIsTrans = false;

    
    
}
//---------------------------------------------------------------------------
inline void NiMAXMaterialConverter::AssignFlipController(
    NiFlipController* pkFlipper, NiTexturingProperty* pkTex, 
    NiTexturingProperty::Map* pkMap)
{
    if (pkFlipper)
    {
        pkFlipper->SetTarget(pkTex);
        pkFlipper->SetAffectedMap(pkMap);
    }
}
//---------------------------------------------------------------------------
