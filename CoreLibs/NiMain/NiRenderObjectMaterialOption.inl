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
inline const NiFixedString& NiRenderObjectMaterialOption::TRANSFORM_SKINNED()
{
    return ms_kStrings[E_TRANSFORM_SKINNED];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderObjectMaterialOption::TRANSFORM_INSTANCED()
{
    return ms_kStrings[E_TRANSFORM_INSTANCED];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderObjectMaterialOption::MORPHING()
{
    return ms_kStrings[E_MORPHING];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderObjectMaterialOption::GetString(
    Enumeration e)
{
    if (e < E__EnumerationCount)
        return ms_kStrings[e];
    else
        return ms_kStrings[E__Invalid];
}
//---------------------------------------------------------------------------
inline NiRenderObjectMaterialOption::Enumeration 
    NiRenderObjectMaterialOption::GetEnumeration(const NiFixedString& kString)
{
    for (unsigned int ui = 0; ui < E__EnumerationCount; ui++)
    {
        if (kString == ms_kStrings[(Enumeration)ui])
            return (Enumeration)ui;
    }
    return E__Invalid;
}
//---------------------------------------------------------------------------
