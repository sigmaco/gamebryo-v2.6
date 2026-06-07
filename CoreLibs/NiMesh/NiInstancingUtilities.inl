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
inline void NiInstancingUtilities::PackTransform(
    const NiTransform& kSourceTransform, NiPoint4* pkDest)
{
    // Note: This method assumes pkDest points to an array of 3 NiPoint4's.
    NiPoint3 kCol0, kCol1, kCol2;

    kSourceTransform.m_Rotate.GetCol(0, kCol0);
    kSourceTransform.m_Rotate.GetCol(1, kCol1);
    kSourceTransform.m_Rotate.GetCol(2, kCol2);

    kCol0 *= kSourceTransform.m_fScale;
    kCol1 *= kSourceTransform.m_fScale;
    kCol2 *= kSourceTransform.m_fScale;

    pkDest[0] = 
        NiPoint4(kCol0.x, kCol0.y, kCol0.z, kSourceTransform.m_Translate.x);

    pkDest[1] = 
        NiPoint4(kCol1.x, kCol1.y, kCol1.z, kSourceTransform.m_Translate.y);

    pkDest[2] = 
        NiPoint4(kCol2.x, kCol2.y, kCol2.z, kSourceTransform.m_Translate.z);
}
//---------------------------------------------------------------------------

