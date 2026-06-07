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

#pragma once

using namespace System;

namespace NiManagedToolInterface
{
    public __gc class MUtility
    {
    public:
        static float GetCurrentTimeInSec();
        static String* GetGamebryoVersion();
        static String* GetGamebryoBuildDate();
        static int GetVersionFromString(String* strVersion);
        static void ScaleToFit(NiAVObject* pkObjToScale,
            NiAVObject* pkObjToMatch, float fPercentage,
            bool bAllowHiddenBoundsForToScale,
            bool bAllowHiddenBoundsForToMatch);

        static void SetImageSubFolder(String* strPath);

        static void GetWorldBounds(NiAVObject* pkObj, NiBound& kBound, 
            bool bAllowHiddenBounds);
    private:
        static void GetWorldBoundsNode(NiNode* pkObj, NiBound& kBound, 
            bool bAllowHiddenBounds);
    };
}
