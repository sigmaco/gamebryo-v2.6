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

#ifndef NIMAXSHADERCLASSDESC_H
#define NIMAXSHADERCLASSDESC_H

//---------------------------------------------------------------------------
class NiMAXShaderClassDesc : public ClassDesc2 
{
public:
    inline int             IsPublic();
    inline void *          Create(BOOL loading);
    inline const TCHAR *   ClassName();
    inline SClass_ID       SuperClassID();
    inline Class_ID        ClassID();
    inline const TCHAR*    Category();

    inline const TCHAR*    InternalName();
    inline HINSTANCE       HInstance();
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXShaderClassDesc.inl"
//---------------------------------------------------------------------------

#endif
