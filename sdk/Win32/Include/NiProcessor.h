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

#ifndef NIPROCESSOR_H
#define NIPROCESSOR_H

#include "NiMainLibType.h"

class NiTransform;
class NiMatrix3;

class NIMAIN_ENTRY NiProcessorSpecificCode
{
public:
    // transforms
    typedef void (*VectorTransformFunc) (NiUInt32 uiVerts,
        const float* pModel, float* pWorld, NiMatrix3* pMatrix);

    typedef void (*PointTransformFunc) (NiUInt32 uiVerts,
        const float* pModel, float* pWorld, const NiTransform* pXform);

    static void TransformPointsBasic (NiUInt32 uiVerts,
        const float* pModel, float* pWorld, const NiTransform* pXform);

    static void TransformVectorsBasic (NiUInt32 uiVerts,
        const float* pModel, float* pWorld, NiMatrix3* pMatrix);

    // These call whichever optimized version that has been installed or the
    // above basic functions if no optimized versions have been installed.
    static void TransformPoints (NiUInt32 uiVerts, const float* pModel,
        float* pWorld, const NiTransform* pXform);

    static void TransformVectors (NiUInt32 uiVerts, const float* pModel,
        float* pWorld, NiMatrix3* pMatrix);

    // These default to Transform*Basic, but can be changed using the
    // following functions to replace them with optimized versions.
    static void SetVectorTransformFunction (VectorTransformFunc vtf);
    static void SetPointTransformFunction (PointTransformFunc ptf);
protected:
    // specialized transformation functions
    static VectorTransformFunc ms_pVectorsTransform;
    static PointTransformFunc ms_pPointsTransform;
};

//---------------------------------------------------------------------------
inline void NiProcessorSpecificCode::TransformPoints (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, const NiTransform* pXform)
{
    (*ms_pPointsTransform)(uiVerts,pModel,pWorld,pXform);
}
//---------------------------------------------------------------------------
inline void NiProcessorSpecificCode::TransformVectors (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, NiMatrix3* pMatrix)
{
    (*ms_pVectorsTransform)(uiVerts,pModel,pWorld,pMatrix);
}
//---------------------------------------------------------------------------
inline void NiProcessorSpecificCode::SetVectorTransformFunction 
    (VectorTransformFunc vtf)
{
    ms_pVectorsTransform = vtf;
}
//---------------------------------------------------------------------------
inline void NiProcessorSpecificCode::SetPointTransformFunction 
    (PointTransformFunc ptf)
{
    ms_pPointsTransform = ptf;
}
//---------------------------------------------------------------------------

#endif // NIPROCESSOR_H
