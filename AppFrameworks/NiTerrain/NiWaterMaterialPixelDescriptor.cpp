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

#include <NiTerrainPCH.h>

#include "NiWaterMaterialPixelDescriptor.h"

//---------------------------------------------------------------------------
NiString NiWaterMaterialPixelDescriptor::ToString()
{
    NiString kResult;
    ToStringTANGENTS(kResult, false);
    ToStringENABLE_SUNLIGHT(kResult, false);
    ToStringUSE_PLANAR_REFLECTION(kResult, false);
    ToStringUSE_CUBE_MAP_REFLECTION(kResult, false);    
    ToStringNUM_TEXCOORDS(kResult, false);
    ToStringREFRACTION_MAP(kResult, false);
    ToStringREFLECTION_MAP(kResult, false);
    ToStringNORMAL_MAP(kResult, false);
    
    return kResult;
}
