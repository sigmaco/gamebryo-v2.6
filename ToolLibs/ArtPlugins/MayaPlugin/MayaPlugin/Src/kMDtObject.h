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

#ifndef KMDTOBJECT_H
#define KMDTOBJECT_H

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>

#include "MDt.h"     
#include "MDtExt.h"

#include "kTDynArray.h"

class kMDtObject
{
        const char  *m_pszName; // Name of object (mostly for debugging)
        const char  *m_pszType; // Type of object (mostly for debugging)
        EType   m_eType;        // Object type
        int     m_iIndex;       // Index into private database

    public:

        MDagPath m_DagPath;
        MObject  m_XformNode;
        MObject  m_ParentNode;

        kTDynArray<int> m_idynParents;
        kTDynArray<int> m_idynChildren;

         kMDtObject( int iIndex, EType eType, MDagPath dp, MObject xnode, 
             MObject parentNode );

         kMDtObject( int iIndex, MDagPath dp, MObject xnode, MObject 
             parentNode );

         kMDtObject(void);
        ~kMDtObject(void);

        EType GetType(void);
        int GetIndex(void);
        int GetTransform(MObject &XformNode);
        int GetRotationWOOrientation( float *x, float *y, float *z);
        int GetRotation( float *x, float *y, float *z);
        int GetPivot( float *x, float *y, float *z);
        int GetPivotTranslation( float *x, float *y, float *z);
        int GetTranslation( float *x, float *y, float *z);
        int GetFinalTranslation( float *x, float *y, float *z);
        int GetFinalNonAnimatingNotUniformScale(float *x, float *y, float *z);
        int GetScale(float *x, float *y, float *z);
        int GetScalePivot(float *x, float *y, float *z);
        int GetScalePivotTranslation(float *x, float *y, float *z);

        int GetJointOrientation( float *x, float *y, float *z);
        int GetRotateAxis( float *x, float *y, float *z);
        int GetWorldTranslate(float *x, float *y, float *z);

        bool GetWorldMatrix(MMatrix &WorldMatrix);
        bool GetWorldMatrix(MTransformationMatrix &WorldMatrix);

        bool AnimatedPlugOnTransform(char* szPlug);

        const char *GetName();
        bool IsObjectSelected();
        bool IsObjectVisible();
};

// Extern the global
extern kTDynArray<kMDtObject> gMDtObjects;

void gMDtObjectAdd( int iIndex, EType eType, MDagPath dp, MObject xnode, 
    MObject parentNode );

#endif