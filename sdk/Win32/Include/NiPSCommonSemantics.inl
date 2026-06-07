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
inline const NiFixedString& NiPSCommonSemantics::PARTICLEPOSITION()
{
    return ms_akSemantics[PS_PARTICLEPOSITION];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEVELOCITY()
{
    return ms_akSemantics[PS_PARTICLEVELOCITY];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEAGE()
{
    return ms_akSemantics[PS_PARTICLEAGE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLELIFESPAN()
{
    return ms_akSemantics[PS_PARTICLELIFESPAN];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLELASTUPDATE()
{
    return ms_akSemantics[PS_PARTICLELASTUPDATE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEFLAGS()
{
    return ms_akSemantics[PS_PARTICLEFLAGS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLERADIUS()
{
    return ms_akSemantics[PS_PARTICLERADIUS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLESIZE()
{
    return ms_akSemantics[PS_PARTICLESIZE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEROTAXIS()
{
    return ms_akSemantics[PS_PARTICLEROTAXIS];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEROTANGLE()
{
    return ms_akSemantics[PS_PARTICLEROTANGLE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLEROTSPEED()
{
    return ms_akSemantics[PS_PARTICLEROTSPEED];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::PARTICLECOLOR()
{
    return ms_akSemantics[PS_PARTICLECOLOR];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPSCommonSemantics::GetSemanticString(
    CommonSemantics eSemantic)
{
    if (eSemantic < PS_SEMANTICSCOUNT)
    {
        return ms_akSemantics[eSemantic];
    }
    else
    {
        return ms_akSemantics[PS_INVALID];
    }
}
//---------------------------------------------------------------------------
