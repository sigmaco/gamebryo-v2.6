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
#include "MSceneObject.h"

namespace NiManagedToolInterface
{
    public __gc class MOrbitPoint
    {
    public:
        MOrbitPoint();
        ~MOrbitPoint();
        void DeleteContents();

        void ResetOrbitPoint();
        void GetOrbitBound(NiBound& kBound);
        void GetWorldTranslate(NiPoint3& kPt);

        void GetParentWorldTransform(NiTransform& kTransform);
        void SetParentWorldTransform(NiTransform kTransform);
        void SetTranslate(NiPoint3 kPt);
        void GetTranslate(NiPoint3 kPt);
        
        void Update(float fTime);
        void Initialize(NiAVObject* pkSceneToFit,
            NiAVObject* pkTransformRelativeTo);

        __property bool get_Active();

        __property bool get_AppCulled();
        __property void set_AppCulled(bool bAppCulled);

        __property bool get_Follow();
        __property void set_Follow(bool bFollow);
        
    protected:
        void LoadOrbitScene();

        NiNode* m_pkOrbitRoot;
        NiNode* m_pkOrbitScene;

        bool m_bFollow;

        NiAVObject* m_pkFollowObj;
        
    };
}
