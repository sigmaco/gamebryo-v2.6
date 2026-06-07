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

#ifndef NIMAXSHAPECONVERTER_H
#define NIMAXSHAPECONVERTER_H


#include "NiMAX.h"
#include "NiMAXConverter.h"

//---------------------------------------------------------------------------
class NiMAXShapeConverter
{
public:
    static int ConvertShape(
        INode* pkMaxNode,
        ShapeObject* pkObj, 
        NiAVObject*& pkLines,
        ScaleInfo* pkScaleInfo);
    static int GetSpline3DVertCount(Spline3D* pkSpline, int iSampleCount);
    static float ApplyScale(
        ScaleInfo* pkParentScale,  
        NiTStridedRandomAccessIterator<NiPoint3> &pkVerts,
        int iNumVerts);
    static void ConvertSpline3D(
        Spline3D* pkSpline,
        NiPoint3* pkVerts, 
        NiBool* pbConnections,
        int iSampleCount);
    static void ConvertShapeMaterial(
        INode* pkMaxNode, 
        NiMaterialProperty* pkMaterial);
private:
};

#endif

