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

#ifndef NIMAXVNORMAL_H
#define NIMAXVNORMAL_H

#include "NiMAX.h"

class NiMAXVNormal : public NiMemObject
{ 
public: 
    Point3 m_kNorm; 
    DWORD m_lSmooth; 
    NiMAXVNormal *m_pkNext; 
    BOOL m_bInit; 

    NiMAXVNormal();
    NiMAXVNormal(const Point3 &normal,DWORD smooth);
    ~NiMAXVNormal();
    void AddNormal(const Point3 &normal,DWORD smooth); 
    Point3 &GetNormal(DWORD smooth); 
    void Normalize(); 
    void FreeNormals();
};

#endif
