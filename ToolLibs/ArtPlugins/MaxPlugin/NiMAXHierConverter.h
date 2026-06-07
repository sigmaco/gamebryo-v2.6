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

#ifndef NIMAXHIERCONVERTER_H
#define NIMAXHIERCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiTHash.h"

//---------------------------------------------------------------------------
class NiMAXHierConverter
{
public:
    NiMAXHierConverter(TimeValue animStart, TimeValue animEnd);
    
    static void Preprocess(INode* pNode);
    static void Postprocess(INode* pNode);

    int BuildNode(
        Interface *pIntf, 
        INode *pNode, 
        NiNodePtr *pspResult, 
        ScaleInfo *pParentScale,
        bool bIgnoreHiddenFlag,
        bool bIgnoreVisAnim);

    int ConvertGeom(
        Object *pObj,
        NiNode *pCurNode,
        INode *pMaxNode,
        Interface *pIntf,
        ScaleInfo *pParentScale,
        bool bIsHidden);

    static void GetOOTM(
        INode *pMaxNode,
        Point3 *pP,
        Quat *pQ, 
        const ScaleInfo *pScIn,
        ScaleInfo *pScOut);

    static Matrix3 GetLocalTM(
        INode *pMaxNode,
        TimeValue t, 
        const ScaleInfo* pScaleIn, 
        ScaleInfo* pScaleOut = NULL);
    static float GetLocalUScale(
        INode *pMaxNode,
        TimeValue t,
        const ScaleInfo* pScaleIn);

    static NiNode *Lookup(INode *pNode, ScaleInfo *pParentScale);
    static void Replace(
        INode *pNode,
        ScaleInfo *pParentScale, 
        NiNode *pNewNode);

    static void Init();
    static void Shutdown();

    static const char* GetCurrentExportObjectName();
    static bool ExportThisNode(INode* pkNode);
    static bool ExportObjectOwnedByNode(INode* pkNode, Object* pObj);
    static bool IsNodeSelected(INode* pkNode);

private:
    int ConvertXform(
        INode *pMaxNode,
        NiAVObject *pAVObj, 
        const ScaleInfo *pScIn,
        ScaleInfo *pScOut);

    static Matrix3 UniformMatrix(Matrix3 orig_cur_mat);
    static void AddSelectedObjectsRecursive(INode* pNode, bool& bAddedNode);

    TimeValue m_animStart, m_animEnd;

    static NiTHash<NiNodePtr>* ms_pkNodeHTab;
    static NiTPointerList<INode *> ms_kSelectedNodes;


    static void *m_pUserData;
    static NiString* ms_pstrCurrentName;
};

//---------------------------------------------------------------------------
#include "NiMAXHierConverter.inl"
//---------------------------------------------------------------------------
#endif
