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

// MDtCollider.h

#ifndef MDTCOLLIDER_H
#define MDTCOLLIDER_H

#include "MDt.h"
#include "MDtExt.h"
#include <NiMatrix3.h>

class kMDtCollider
{
public:

    char*       m_szName;
    MObject     m_MObjectParticles;
    MObject     m_MObjectColliderTransform;

    // Transform Attributes
    float       m_fTranslateX;
    float       m_fTranslateY;
    float       m_fTranslateZ;
    float       m_fRotateX;
    float       m_fRotateY;
    float       m_fRotateZ;
    float       m_fScaleX;
    float       m_fScaleY;
    float       m_fScaleZ;

    float       m_fWorldTranslateX;
    float       m_fWorldTranslateY;
    float       m_fWorldTranslateZ;
    float       m_fWorldRotateX;
    float       m_fWorldRotateY;
    float       m_fWorldRotateZ;
    float       m_fWorldScaleX;
    float       m_fWorldScaleY;
    float       m_fWorldScaleZ;

    float       m_fWidth;
    float       m_fHeight;
    float       m_fRadius;

    float       m_fResilience;
    float       m_fFriction;

    NiMatrix3   m_kWorldRotationMatrix;

    kMDtCollider();
    ~kMDtCollider();

    bool Load(MObject MObjectParticleName, int iMeshID);
    void ScaleForLinearUnits(float fUnitMultiplier);

protected:

    MObject FindConnectedDependency(MObject CurrentDependency, 
        char* szPlugName, int iElement = -1);

    void CreateNameFromPolyName(char* szName, unsigned int uiNameLen);
    bool GetAttribute(char* szObject, char* szAttribute, float& fResult);

    bool GetWorldAttributes(char* szObject);
};

#endif