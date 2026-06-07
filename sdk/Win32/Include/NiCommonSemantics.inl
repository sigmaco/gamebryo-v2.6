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
inline const NiFixedString& NiCommonSemantics::POSITION()
{
    return ms_kStrings[E_POSITION];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::NORMAL()
{
    return ms_kStrings[E_NORMAL];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BINORMAL()
{
    return ms_kStrings[E_BINORMAL];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::TANGENT()
{
    return ms_kStrings[E_TANGENT];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::TEXCOORD()
{
    return ms_kStrings[E_TEXCOORD];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BLENDWEIGHT()
{
    return ms_kStrings[E_BLENDWEIGHT];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BLENDINDICES()
{
    return ms_kStrings[E_BLENDINDICES];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::COLOR()
{
    return ms_kStrings[E_COLOR];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::PSIZE()
{
    return ms_kStrings[E_PSIZE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::TESSFACTOR()
{
    return ms_kStrings[E_TESSFACTOR];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::DEPTH()
{
    return ms_kStrings[E_DEPTH];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::FOG()
{
    return ms_kStrings[E_FOG];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::POSITIONT()
{
    return ms_kStrings[E_POSITIONT];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::SAMPLE()
{
    return ms_kStrings[E_SAMPLE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::DATASTREAM()
{
    return ms_kStrings[E_DATASTREAM];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::INDEX()
{
    return ms_kStrings[E_INDEX];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BONEMATRICES()
{
    return ms_kStrings[E_BONEMATRICES];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BONE_PALETTE()
{
    return ms_kStrings[E_BONE_PALETTE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::POSITION_BP()
{
    return ms_kStrings[E_POSITION_BP];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::NORMAL_BP()
{
    return ms_kStrings[E_NORMAL_BP];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::BINORMAL_BP()
{
    return ms_kStrings[E_BINORMAL_BP];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::TANGENT_BP()
{
    return ms_kStrings[E_TANGENT_BP];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::MORPHWEIGHTS()
{
    return ms_kStrings[E_MORPHWEIGHTS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::NORMALSHAREINDEX()
{
    return ms_kStrings[E_NORMALSHAREINDEX];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::NORMALSHAREGROUP()
{
    return ms_kStrings[E_NORMALSHAREGROUP];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::TRANSFORMS()
{
    return ms_kStrings[E_TRANSFORMS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::INSTANCETRANSFORMS()
{
    return ms_kStrings[E_INSTANCETRANSFORMS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::DISPLAYLIST()
{
    return ms_kStrings[E_DISPLAYLIST];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiCommonSemantics::GetString(Enumeration e)
{
    if (e < E__EnumerationCount)
        return ms_kStrings[e];
    else
        return ms_kStrings[E__Invalid];
}
//---------------------------------------------------------------------------
inline NiCommonSemantics::Enumeration NiCommonSemantics::GetEnumeration(
    const NiFixedString& kString)
{
    for (unsigned int ui = 0; ui < E__EnumerationCount; ui++)
    {
        if (kString == ms_kStrings[(Enumeration)ui])
            return (Enumeration)ui;
    }
    return E__Invalid;
}
//---------------------------------------------------------------------------
