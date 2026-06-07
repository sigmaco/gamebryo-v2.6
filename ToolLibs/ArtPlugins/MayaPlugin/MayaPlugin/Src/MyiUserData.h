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

// MyiUserData.h

#ifndef MYIUSERDATA_H
#define MYIUSERDATA_H

#include <NiObjectNet.h>

#include <maya/MFnDagNode.h> 

class NiNode;

class sUserData
{
public:
    sUserData();
    ~sUserData();
    void Reset(void);

    NiNode *m_pNode;
    NiNode *m_pParent;

    // NULL is a legitimate value for m_pParent, so we cannot compare
    // m_pParent to NULL to figure out whether it has been set. So
    // we use this boolean.
    bool m_bHasParent;

};

class MyiUserData
{
    protected:
        sUserData *m_pUserData;
        int        m_iMaxShape;

    public:
        void InitSize(int iSize);
        MyiUserData(int iSize = 0);
        ~MyiUserData(void);
        void SetNode(int iShapeNum,   NiNode *pNode);
        void SetParent(int iShapeNum, NiNode *pParent);
        NiNode *GetNode(int iShapeNum);
        NiNode *GetParent(int iShapeNum);
        bool HasParent(int iShapeNum);
        void Reset(void);
        bool ElementGetUserData( int iShapeNum, sUserData *pUser);    
        bool ElementDestroyUserData( int iShapeNum );
        bool ElementSetUserData( int iShapeNum, sUserData *pUser );
        int GetShapeNum(NiNode* pNode);
        NiNode* GetNode(MObject kObject);
};

extern MyiUserData gUserData;



void AttachUserDefinedAttributes( MFnDagNode& dgNode, NiObjectNET *pObject );

bool CheckForExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase);

bool CheckForExtraAttribute(MFnDependencyNode& kDepNode, 
    char *szAttributeName, bool bIgnoreCase);

bool GetExtraDataAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, MFnAttribute& FoundAttribute);

bool GetExtraDataAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, MFnAttribute& FoundAttribute);


bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, bool &bValue);

bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, int &iValue);

bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, float &fValue);

bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, double &dValue);

bool GetExtraAttribute(MFnDagNode& dgNode, char *szAttributeName, 
    bool bIgnoreCase, MString sValue);


bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, bool &bValue);

bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, int &iValue);

bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName,
    bool bIgnoreCase, float &fValue);
bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, double &dValue);

bool GetExtraAttribute(MFnDependencyNode& kDepNode, char *szAttributeName, 
    bool bIgnoreCase, MString &sValue);

#endif
