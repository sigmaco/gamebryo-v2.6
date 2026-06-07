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

#include "MaxImmerse.h"
#include "NiUtils.h"
#include "NiMAXShapeConverter.h"
#include <NiBool.h>

//---------------------------------------------------------------------------
int NiMAXShapeConverter::ConvertShape(
    INode* pkMaxNode,
    ShapeObject* pkObj, 
    NiAVObject*& pkLines,
    ScaleInfo* pkScaleInfo)
{
    BezierShape kBezier;
    Spline3D* pkSpline;

    int iVertCount = 0;
    int iVertCounter = 0;
    int iSampleCount = 10;

    if (pkObj->CanMakeBezier())
    {
        pkObj->MakeBezier(0, kBezier);
        for (int i = 0; i < kBezier.SplineCount(); i++)
        {
            pkSpline = kBezier.GetSpline(i);
            if (pkSpline)
                iVertCount += GetSpline3DVertCount(pkSpline, iSampleCount);
        }
        
        pkLines = NiNew NiMesh();
        NiMesh* pkMesh=NiDynamicCast(NiMesh, pkLines);

        NiDataStreamElementLock kVertsLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::POSITION(), 
            0,
            NiDataStreamElement::F_FLOAT32_3,   
            iVertCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX,
            true); 

        NiTStridedRandomAccessIterator<NiPoint3> pkVerts =
            kVertsLock.begin<NiPoint3>(0); 

        NiBool* pbConnections = NiAlloc(NiBool, iVertCount);
        
        for (int i = 0; i < kBezier.SplineCount(); i++)
        {
            pkSpline = kBezier.GetSpline(i);
            if (pkSpline)
            {
                ConvertSpline3D(pkSpline, &pkVerts[iVertCounter], 
                    &pbConnections[iVertCounter], iSampleCount);
                iVertCounter += GetSpline3DVertCount(pkSpline, iSampleCount);
            }
        }

        float fScale;
        fScale = ApplyScale(pkScaleInfo, pkVerts, iVertCount);        

        pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

        kVertsLock.Unlock();

        NiUInt32 uiNrConnections=0;
        for(unsigned int i=0;(int)i<iVertCount;i++)
        {
            if (pbConnections[i])
                uiNrConnections++;
        }

        NiDataStreamElementLock kIndexLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::INDEX(), 
            0,
            NiDataStreamElement::F_UINT32_1,
            uiNrConnections*2, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            true);

        NiTStridedRandomAccessIterator<NiUInt32> pIndices =
            kIndexLock.begin<NiUInt32>(0); 

        NiUInt32 uiIndex = 0;

        for(unsigned int i=0;(int)i<iVertCount;i++)
        {
            if (!pbConnections[i])
                continue;

            pIndices[uiIndex++] = i;

            if (((int)i + 1) == iVertCount)
            {
                pIndices[uiIndex++] = 0;
                break;
            }
            else
            {
                pIndices[uiIndex++] = i + 1;
            }
        }

        kIndexLock.Unlock();

        NiFree(pbConnections);

        pkMesh->RecomputeBounds(); 
        pkLines->SetScale(fScale);

        NiMaterialProperty* pkMaterial;
        pkMaterial = NiNew NiMaterialProperty;
        NIASSERT(pkMaterial);

        ConvertShapeMaterial(pkMaxNode, pkMaterial);

        pkLines->AttachProperty(pkMaterial);

        return W3D_STAT_OK;
    }
    else
    {
        return W3D_STAT_FAILED;
    }
}
//---------------------------------------------------------------------------
int NiMAXShapeConverter::GetSpline3DVertCount(
    Spline3D* pkSpline, 
    int iSampleCount)
{
    // first, go through the knots and count the number of verts
    // we will need to create a reasonable representation
    int iVertCount = 0;
    SplineKnot* pkCurrentKnot;
    SplineKnot* pkNextKnot;

    for (int i = 0; i < pkSpline->KnotCount() - 1; i++)
    {
        SplineKnot kCurrentKnot = pkSpline->GetKnot(i);
        SplineKnot kNextKnot = pkSpline->GetKnot(i + 1);
        pkCurrentKnot = &kCurrentKnot;
        pkNextKnot = &kNextKnot;
        
        if ((pkCurrentKnot) && (pkNextKnot))
        {
            if ((pkCurrentKnot->Ktype() == KTYPE_CORNER) &&
                (pkNextKnot->Ktype() == KTYPE_CORNER))
            {
                iVertCount++;
            }
            else
            {
                iVertCount += iSampleCount;
            }
        }
    }

    if (pkSpline->Closed())
    {
        SplineKnot kCurrentKnot = pkSpline->GetKnot(pkSpline->KnotCount() - 1);
        SplineKnot kNextKnot = pkSpline->GetKnot(0);
        pkCurrentKnot = &kCurrentKnot;
        pkNextKnot = &kNextKnot;

        if ((pkCurrentKnot->Ktype() == KTYPE_CORNER) &&
            (pkNextKnot->Ktype() == KTYPE_CORNER))
        {
            iVertCount++;
        }
        else
        {
            iVertCount += iSampleCount;
        }
        iVertCount++;
    }
    else
    {
        iVertCount++;
    }

    return iVertCount;
}
//---------------------------------------------------------------------------
float NiMAXShapeConverter::ApplyScale(
    ScaleInfo* pkParentScale, 
    NiTStridedRandomAccessIterator<NiPoint3> &pkVerts,
    int iNumVerts)
{
    if (pkParentScale == NULL)
        return 1.0f;

    if (pkParentScale->IsAnisotropic())
    {
        Matrix3 srtm, stm, srtminv, tm;
        Point3 p1, p2;
        Quat qinv;        

        stm = ScaleMatrix(pkParentScale->scale.s);
        pkParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pkParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;

        for (int i = 0; i < iNumVerts; i++)
        {
            NiPoint3& vert = pkVerts[i];
            p1.x = vert.x;
            p1.y = vert.y;
            p1.z = vert.z;

            p2 = p1 * tm;

            vert.x = p2.x;
            vert.y = p2.y;
            vert.z = p2.z;
        }
        return 1.0f;
    }
    return pkParentScale->scale.s.x;
}
//---------------------------------------------------------------------------
void NiMAXShapeConverter::ConvertSpline3D(
    Spline3D* pkSpline, 
    NiPoint3* pkVerts,
    NiBool* pbConnections,
    int iSampleCount)
{
    SplineKnot* pkCurrentKnot;
    SplineKnot* pkNextKnot;
    int iVertCounter = 0;

    for (int i = 0; i < pkSpline->KnotCount() - 1; i++)
    {
        SplineKnot kCurrentKnot = pkSpline->GetKnot(i);
        SplineKnot kNextKnot = pkSpline->GetKnot(i + 1);
        pkCurrentKnot = &kCurrentKnot;
        pkNextKnot = &kNextKnot;

        if ((pkCurrentKnot) && (pkNextKnot))
        {
            if ((pkCurrentKnot->Ktype() == KTYPE_CORNER) &&
                (pkNextKnot->Ktype() == KTYPE_CORNER))
            {
                pkVerts[iVertCounter].x = pkCurrentKnot->Knot().x;
                pkVerts[iVertCounter].y = pkCurrentKnot->Knot().y;
                pkVerts[iVertCounter].z = pkCurrentKnot->Knot().z;
                pbConnections[iVertCounter] = true;
                iVertCounter++;
            }
            else
            {
                for (int j = 0; j < iSampleCount; j++)
                {
                    Point3 kInterpPoint;
                    kInterpPoint = pkSpline->InterpBezier3D(i, 
                        ((float)j / (float)iSampleCount));

                    pkVerts[iVertCounter].x = kInterpPoint.x;
                    pkVerts[iVertCounter].y = kInterpPoint.y;
                    pkVerts[iVertCounter].z = kInterpPoint.z;
                    pbConnections[iVertCounter] = true;
                    iVertCounter++;
                }
            }
        }
    }
    
    if (pkSpline->Closed())
    {
        SplineKnot kCurrentKnot = pkSpline->GetKnot(pkSpline->KnotCount() - 1);
        SplineKnot kNextKnot = pkSpline->GetKnot(0);
        pkCurrentKnot = &kCurrentKnot;
        pkNextKnot = &kNextKnot;

        if ((pkCurrentKnot->Ktype() == KTYPE_CORNER) &&
            (pkNextKnot->Ktype() == KTYPE_CORNER))
        {
            pkVerts[iVertCounter].x = pkCurrentKnot->Knot().x;
            pkVerts[iVertCounter].y = pkCurrentKnot->Knot().y;
            pkVerts[iVertCounter].z = pkCurrentKnot->Knot().z;
            pbConnections[iVertCounter] = true;
            iVertCounter++;
        }
        else
        {
            for (int j = 0; j < iSampleCount; j++)
            {
                Point3 kInterpPoint;
                kInterpPoint = pkSpline->InterpBezier3D(
                    pkSpline->KnotCount() - 1, 
                    ((float)j / (float)iSampleCount));

                pkVerts[iVertCounter].x = kInterpPoint.x;
                pkVerts[iVertCounter].y = kInterpPoint.y;
                pkVerts[iVertCounter].z = kInterpPoint.z;
                pbConnections[iVertCounter] = true;
                iVertCounter++;
            }
        }
        SplineKnot kCurrentKnot0 = pkSpline->GetKnot(0);
        pkCurrentKnot = &kCurrentKnot0;
        pkVerts[iVertCounter].x = pkCurrentKnot->Knot().x;
        pkVerts[iVertCounter].y = pkCurrentKnot->Knot().y;
        pkVerts[iVertCounter].z = pkCurrentKnot->Knot().z;
        pbConnections[iVertCounter] = false;
    }
    else
    {
        SplineKnot kCurrentKnot = pkSpline->GetKnot(pkSpline->KnotCount() - 1);
        pkCurrentKnot = &kCurrentKnot;
        pkVerts[iVertCounter].x = pkCurrentKnot->Knot().x;
        pkVerts[iVertCounter].y = pkCurrentKnot->Knot().y;
        pkVerts[iVertCounter].z = pkCurrentKnot->Knot().z;
        pbConnections[iVertCounter] = false;
        iVertCounter++;
    }
}
//---------------------------------------------------------------------------
void NiMAXShapeConverter::ConvertShapeMaterial(
    INode* pkMaxNode, 
    NiMaterialProperty* pkMaterial)
{
    DWORD lWireColor;
    float fRed, fGreen, fBlue;

    lWireColor = pkMaxNode->GetWireColor();
    fRed = (float)((lWireColor & 0x000000ff) / 255.0f);
    fGreen = (float)(((lWireColor & 0x0000ff00) >> 8) / 255.0f);
    fBlue = (float)(((lWireColor & 0x00ff0000) >> 16) / 255.0f);

    pkMaterial->SetAmbientColor(NiColor(0.0f, 0.0f, 0.0f));
    pkMaterial->SetDiffuseColor(NiColor(0.0f, 0.0f, 0.0f));
    pkMaterial->SetSpecularColor(NiColor(0.0f, 0.0f, 0.0f));
    pkMaterial->SetEmittance(NiColor(fRed, fGreen, fBlue));
    pkMaterial->SetShineness(0.0f);
    pkMaterial->SetAlpha(1.0f);
}
//---------------------------------------------------------------------------
