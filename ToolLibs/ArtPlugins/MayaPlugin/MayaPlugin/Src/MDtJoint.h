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

// MDtJoint.h

#ifndef MDTJOINT_H
#define MDTJOINT_H

#include "MDt.h"
#include "MDtExt.h"

#include <maya/MTransformationMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MMatrix.h>

class kMDtJointInfo
{
    public:
        kMDtJointInfo(void);
        ~kMDtJointInfo(void);

        float    m_fSkinToBoneMat[4][4];
        int      m_iJointIndex;
};

class kMDtJointInfoManager
{
    public:
        int             m_iJoints;
        int             m_iAllocated;
        kMDtJointInfo **m_ppJoints;
        void            DoubleSpace(void);

        int Add( kMDtJointInfo *pJ );
        kMDtJointInfo* Get( int iIndex );
        void GetAllShapeJoints( int iShapeNum, int *iNumJoints, 
            int **piJoints );
        void GetSkinToBoneMat( int iJoint, float mat[4][4]);

        kMDtJointInfoManager(void);
        ~kMDtJointInfoManager(void);
};

class kMDtJointXform
{
    public:
        MObject  m_MObjectJoint;
        MObject  m_MObjectParent;
};

class kMDtJointXformManager
{
    private:
        int m_iAllocated;
        kMDtJointXform **m_ppJointXforms;
        void DoubleSpace(void);

    // May be expanded to insert, delete, etc... if necessary.
    // if so, we should add into fields that are NULL instead 
    // of just appending...
    public:
        int m_iJointXforms;

        kMDtJointXformManager(void);
        ~kMDtJointXformManager(void);
        int Add( kMDtJointXform *pJ );
        kMDtJointXform* Get( int iIndex );
        int GetJoint( MObject mobj );       
        void Reset(void);
        MObject& GetJointXformNode(int iJoint);
};

extern kMDtJointXformManager gJointXformMan;

#endif