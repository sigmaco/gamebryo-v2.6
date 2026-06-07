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

#ifndef MDTMULTPARENTS_H
#define MDTMULTPARENTS_H

#include "MDt.h"
#include "MDtExt.h"

class kMDtMultParentInfo
{
    public:
        MObject m_Node; // This node has multiple parents
        int iNumParents;// The number of parents this node has.
        int iCurrent;   // As we process, we'll bump up the current parent
};

class kMDtMultParentManager
{
    private:
        int m_iAllocated;
        kMDtMultParentInfo **m_ppMultParentInfo;
        void DoubleSpace(void);

    // May be expanded to insert, delete, etc... if necessary.
    // if so, we should add into fields that are NULL instead 
    // of just appending...
    public:
        int m_iUsed;

        kMDtMultParentManager(void);
        ~kMDtMultParentManager(void);
        //int Add( kMDtMultParentInfo *pJ );
        int Add( MObject &mObj, int iParentCnt );
        kMDtMultParentInfo* Get( int iIndex );
        void Reset(void);
};

extern kMDtMultParentManager gMultParentMan;

#endif
