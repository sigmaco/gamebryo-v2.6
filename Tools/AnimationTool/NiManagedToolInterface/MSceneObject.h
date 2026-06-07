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

namespace NiManagedToolInterface
{
    public __gc class MSceneObject
    {
    public:
        MSceneObject(NiAVObject* pkObject);

        // Properties.
        __property String* get_Name();
        __property String* get_RTTIName();

        MSceneObject* GetChildren()[];

        NiAVObject* GetObject();

        static bool IsToolOnlyObject(NiAVObject* pkObject);
        static void SetToolOnlyObject(NiAVObject* pkObject, bool bIsToolOnly);

    protected:
        NiAVObject* m_pkObject;
    };
}
