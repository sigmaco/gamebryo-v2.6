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
inline const NiFixedString& NiPhysXPSCommonSemantics::PARTICLEACTORPOSE()
{
    return ms_akSemantics[PHYSXPS_ACTORPOSE];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPhysXPSCommonSemantics::PARTICLEACTORVELOCITY()
{
    return ms_akSemantics[PHYSXPS_ACTORVELOCITY];
}
//---------------------------------------------------------------------------
inline 
const NiFixedString& NiPhysXPSCommonSemantics::PARTICLEACTORLASTVELOCITY()
{
    return ms_akSemantics[PHYSXPS_ACTORLASTVELOCITY];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPhysXPSCommonSemantics::GetSemanticString(
    CommonSemantics eSemantic)
{
    if (eSemantic < PHYSXPS_SEMANTICSCOUNT)
    {
        return ms_akSemantics[eSemantic];
    }
    else
    {
        return ms_akSemantics[PHYSXPS_INVALID];
    }
}
//---------------------------------------------------------------------------
