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

#ifndef NIMAXMORPHCONVERTER_H
#define NIMAXMORPHCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiTHash.h"

//---------------------------------------------------------------------------
class NiMAXMorphConverter
{
public:
    NiMAXMorphConverter(TimeValue animStart, TimeValue animEnd);
    ~NiMAXMorphConverter() {};

    static bool IsMorphObject(INode *pMaxNode);

    int Convert(
        INode *pMaxNode, 
        NiNode *pCurNode, 
        ScaleInfo *pParentScale);

    static void Preprocess(INode* pNode);
    static void Postprocess(INode* pNode);

    static void Init();
    static void Shutdown();

private:
    TimeValue m_animStart, m_animEnd;

    static Modifier *FindMorpherModifier(Object *pObj);
    bool ConvertBaseObject(
        INode *pMaxNode,
        NiNode *pNode,
        ScaleInfo *pParentScale,
        unsigned short **& ppVertexRemapping,
        NiAVObject**& ppRemapTargets,
        unsigned int& uiNumRemapTargets,
        NiUInt32**& ppuiNewToOld);
    void ConvertMorpher(
        MorphR3 *pMorpher,
        INode *pMaxNode,
        NiNode *pCurNode,
        ScaleInfo *pParentScale);

    static NiTHash<NiAVObjectPtr>* ms_pkMorphHTab;

    static void *m_pUserData;
};


#endif

