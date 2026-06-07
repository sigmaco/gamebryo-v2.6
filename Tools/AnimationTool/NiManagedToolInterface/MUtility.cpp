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

#include "stdafx.h"

#include "MUtility.h"
#include "MSceneObject.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
float MUtility::GetCurrentTimeInSec()
{
    return NiGetCurrentTimeInSec();   
}
//---------------------------------------------------------------------------
String* MUtility::GetGamebryoVersion()
{
    return NiExternalNew String(GAMEBRYO_SDK_VERSION_STRING);
}
//---------------------------------------------------------------------------
String* MUtility::GetGamebryoBuildDate()
{
    return NiExternalNew String(GAMEBRYO_BUILD_DATE_STRING);
}
//---------------------------------------------------------------------------
int MUtility::GetVersionFromString(String* strVersion)
{
    const char* pcVersionString = MStringToCharPointer(strVersion);
    int iVersion = NiStream::GetVersionFromString(pcVersionString);
    MFreeCharPointer(pcVersionString);
    return iVersion;
}
//---------------------------------------------------------------------------
void MUtility::ScaleToFit(NiAVObject* pkObjToScale, NiAVObject* pkObjToMatch,
    float fPercentage, bool bAllowHiddenBoundsForToScale,
    bool bAllowHiddenBoundsForToMatch)
{
    if (pkObjToScale == NULL || pkObjToMatch == NULL)
        return;
    
    if (fPercentage <= 0.0f)
        return;

    float fTime = MFramework::Instance->Clock->CurrentTime;
    pkObjToMatch->Update(fTime);
    NiMesh::CompleteSceneModifiers(pkObjToMatch);
    pkObjToScale->Update(fTime);
    NiMesh::CompleteSceneModifiers(pkObjToScale);

    NiBound kMatchBound;
    GetWorldBounds(pkObjToMatch, kMatchBound, bAllowHiddenBoundsForToMatch);
    NiBound kCurBound;
    GetWorldBounds(pkObjToScale, kCurBound, bAllowHiddenBoundsForToScale);

    float fTargetRadius = fPercentage * kMatchBound.GetRadius();
    float fScale = fTargetRadius / kCurBound.GetRadius();
    pkObjToScale->SetScale(fScale);
}
//---------------------------------------------------------------------------
void MUtility::GetWorldBounds(NiAVObject* pkObj, NiBound& kBound, 
    bool bAllowHiddenBounds)
{
    kBound.SetRadius(0.0f);

    if (!bAllowHiddenBounds && MSceneObject::IsToolOnlyObject(pkObj))
        return;

    if (!bAllowHiddenBounds && pkObj->GetAppCulled())
        return;

    NIASSERT(NiIsKindOf(NiTriBasedGeom, pkObj) == false);
    if (NiIsKindOf(NiNode, pkObj))
    {
        GetWorldBoundsNode((NiNode*)pkObj, kBound, bAllowHiddenBounds);
    }
    else if (NiIsKindOf(NiMesh, pkObj))
    {
        NiMesh* pkMesh = (NiMesh*) pkObj;
        kBound = pkMesh->GetWorldBound();
    }
}
//---------------------------------------------------------------------------
void MUtility::GetWorldBoundsNode(NiNode* pkObj, NiBound& kBound, 
    bool bAllowHiddenBounds)
{
    NiBound kWorldBound;
    kWorldBound.SetRadius(0.0f);

    for (unsigned int i = 0; i < pkObj->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkObj->GetAt(i);
        if (pkChild)
        {
            NiBound kChildBound;
            GetWorldBounds(pkChild, kChildBound, bAllowHiddenBounds);
            
            if (kChildBound.GetRadius() > 0.0f)
            {
                if (kWorldBound.GetRadius() == 0.0f)
                {
                    kWorldBound = kChildBound;
                }
                else
                {
                    kWorldBound.Merge(&kChildBound);
                }
            }
        }
    }

    kBound = kWorldBound;
}
//---------------------------------------------------------------------------
void MUtility::SetImageSubFolder(String* strPath)
{
    const char* pcPath = MStringToCharPointer(strPath);
    NiDevImageConverter::SetPlatformSpecificSubdirectory(pcPath);
    MFreeCharPointer(pcPath);
    
}
//---------------------------------------------------------------------------
