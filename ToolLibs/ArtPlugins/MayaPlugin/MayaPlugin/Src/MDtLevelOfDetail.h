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

// MDtParticleSystem.h

#ifndef MDTLEVELOFDETAIL_H
#define MDTLEVELOFDETAIL_H

#include "MDt.h"
#include "MDtExt.h"



class kMDtLevelOfDetail
{
public:
    kMDtLevelOfDetail();
    ~kMDtLevelOfDetail();


    bool Load(MObject LOD, MObject Parent);


    long        m_lShapeIndex;
    char*       m_szName;
    MObject     m_MObjectLOD;
    MObject     m_MObjectParent;

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

    long        m_iNumThresholds;
    float*      m_pfThresholds;

    bool        m_bUseScreenLOD;
    int         m_iNumScreenThresholds;
    float       m_afScreenThresholds[10];

};



//---------------------------------------------------------------------------

class kMDtLevelOfDetailManager
{
public:

    kMDtLevelOfDetailManager();
    ~kMDtLevelOfDetailManager();

    void Reset();

    int GetNumLevelOfDetail();

    int AddLevelOfDetail(kMDtLevelOfDetail* pNewEmitter);
    kMDtLevelOfDetail* GetLevelOfDetail( int iIndex );
    int GetLevelOfDetail( MObject mobj );       


protected:

    int             m_iNumLevelOfDetail;
    kMDtLevelOfDetail** m_ppLevelOfDetail;

};


//---------------------------------------------------------------------------

int addLevelOfDetail(MObject LODNode, MObject ParentNode);

//---------------------------------------------------------------------------

void gLevelOfDetailNew();   // INITIALIZE AT THE START OF THE EXPORTER

//---------------------------------------------------------------------------

// Declare a global Joint Manager.
extern kMDtLevelOfDetailManager gLevelOfDetailManager;

#endif