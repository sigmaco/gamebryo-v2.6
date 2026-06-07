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

// NiCollisionLabInfo.h

#ifndef NICOLLISIONLABINFO_H
#define NICOLLISIONLABINFO_H

#include <NiPoint3.h>
#include <NiCollisionGroup.h>

class NiCollisionLabInfo
{
    public:
        NiCollisionLabInfo();

        void SetAuxCallbacksOnSceneGraph(bool bValue);
        void RecursiveSetSelectives(NiAVObject* pkObj,
            bool bSelective, bool bTrans, bool bProps, bool bRigid);

        void ResetCollisionIndicator(bool bDynamic);
        void CreateCollisionIndicator();

        bool AddCollisionLabNifFile(CString strPathName, int iLoc);

        NiNode* AddCollisionLabObject(NiAVObject* pkRootObject,
            const char* pcFilePath, int iLoc);

        void RecursiveSetWireABV(NiAVObject* pkObj,
            bool bCull, bool bSelective, bool bTransform,
            bool bRigid, bool bUpdateChild = false);

        // Accessor functions
        bool GetDynamicSim();
        float GetRemainingDistance();
        NiPoint3 GetDirection();
        bool GetCollisionState();
        float GetVelocity();
        bool GetAuxCallbacks();
        float GetIntersectionTime();
        bool GetUpdate() { return m_bUpdate; }
        float GetSmallestObjRadius();

        // Setter functions
        void SetDynamicSim(bool bValue);
        void SetRemainingDistance(float fValue);
        void SetDirection(NiPoint3& kDir);
        void SetCollisionState(bool bState);
        void SetVelocity(float fValue);
        void SetAuxCallbacks(bool bValue);
        void SetUpdate(bool bUpdate) { m_bUpdate = bUpdate; };

        void HideWireFrames();
        void CreateCollisionTriangles(bool bFinalCallFromOutsideCallback);
        bool DisplayTrianglesProcessed() { return false; }

        void ClearIntersectData();
        
        NiAVObject* FindObjectByName(char* pcName);

    protected:
        static int Callback(NiCollisionGroup::Intersect& kIntersect);

        void RecursiveSetAuxCallbacks(NiAVObject* pkObj, bool bEnable);
        void CreateAttachTri( const NiPoint3* apkTri, float fMult,
            NiColor& kColor, float fTime);

        bool m_bDynamicSimOn;
        bool m_bCollided;
        bool m_bAuxCallbacks;
        bool m_bUpdate;

        float m_fRemainingDistance;  
        float m_fVelocity;

        NiPoint3 m_kDirection;
        NiCollisionGroup::Intersect m_kIntersectData;

        // Saved results of colliding tri's.
        NiPoint3 m_kTri1[3];
        NiPoint3 m_kTri2[3];
};

#define COLLIDER_POS 0
#define COLLIDEE_POS 1
#define INDICATOR_POS 2
#define INDICATOR_STATIC_POS 0
#define INDICATOR_DYNAMIC_POS 1

#define STR_COLLISION_WIREABV "AssetViewerCollisionWireABV"
#define STR_COLLISION_LAB "Gamebryo Collision Lab"
#define STR_COLLISION_WIRETRIOBB "AssetViewerCollisionWireTriOBB"
#define STR_COLLISION_COLLIDER "Collider"
#define STR_COLLISION_COLLIDEE "Collidee"
#define STR_COLLISION_INDICATORS "Indicators"
#define STR_COLLISION_STATIC "Static"
#define STR_COLLISION_DYNAMIC "Dynamic"

#define TIME_TOLERANCE 1e-03f
#define DIST_TOLERANCE 1e-04f

#include "NiCollisionLabInfo.inl"

#endif  // #ifndef NICOLLISIONLABINFO