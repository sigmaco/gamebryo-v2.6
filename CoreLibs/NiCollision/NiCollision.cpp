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

// Precompiled Header
#include "NiCollisionPCH.h"

#include <NiVersion.h>
#include "NiCollision.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiCollision);
//---------------------------------------------------------------------------


// Volume types in NiMain:
//  0 = sphere
// Volume types in NiCollision:
//  1 = obb 
//  2 = capsule
//  3 = UNUSED 
//  4 = union
//  5 = half space


NiSphereBV NiCollisionConvert::ms_kSphereABV;
//---------------------------------------------------------------------------
// "Fast" version returns a reference to static member data for speed.
NiBoundingVolume* NiCollisionConvert::ConvertToSphereBV_Fast(float fRadius, 
    const NiPoint3& kCenter)
{
    ms_kSphereABV.SetRadius(fRadius);
    ms_kSphereABV.SetCenter(kCenter);
    return &ms_kSphereABV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiCollisionConvert::ConvertToSphereBV_Safe(float fRadius, 
    const NiPoint3& kCenter)
// "Safe" version returns a pointer to an allocated NiSphereBV.
{
    NiSphereBV* pkSphereBV = NiNew NiSphereBV;

    pkSphereBV->SetRadius(fRadius);
    pkSphereBV->SetCenter(kCenter);

    return pkSphereBV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiCollisionConvert::ConvertToBoxBV(
    unsigned short usQuantity, const NiPoint3* pkVertex)
{
    NIASSERT(usQuantity > 0 && pkVertex);
    if (usQuantity == 0 || pkVertex == 0)
        return 0;

    NiBox kBox;
    kBox.CreateFromData(usQuantity, pkVertex);

    NiBoxBV* pkBoxBV = NiNew NiBoxBV;
    pkBoxBV->SetBox(kBox);

    return pkBoxBV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiCollisionConvert::ConvertToBoxBV(NiMesh* pkMesh)
{
    NiBox kBox;
    kBox.CreateFromData(pkMesh);

    NiBoxBV* pkBoxBV = NiNew NiBoxBV;
    pkBoxBV->SetBox(kBox);

    return pkBoxBV;
}
//---------------------------------------------------------------------------
