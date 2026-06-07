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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiTextureTransform.h"

const NiTextureTransform NiTextureTransform::IDENTITY(
    NiPoint2(0.0f, 0.0f), 
    0.0f,
    NiPoint2(1.0f, 1.0f), 
    NiPoint2(0.5f, 0.5f),
    NiTextureTransform::MAYA_TRANSFORM);

//---------------------------------------------------------------------------
NiTextureTransform::NiTextureTransform(const NiPoint2& kTranslate,
    float fRotate, const NiPoint2& kScale, const NiPoint2& kCenter,
    TransformMethod eMethod)
{
    m_kTranslate = kTranslate;
    m_fRotate = fRotate;
    m_kScale = kScale;
    m_kCenter = kCenter;
    m_eMethod = eMethod;

    UpdateMatrix();
}
//---------------------------------------------------------------------------
void NiTextureTransform::MakeIdentity()
{
    m_kTranslate.x = 0.0f;
    m_kTranslate.y = 0.0f;

    m_kScale.x = 1.0f;
    m_kScale.y = 1.0f;

    m_fRotate = 0.0f;

    m_kCenter.x = 0.5f;
    m_kCenter.y = 0.5f;

    m_eMethod = NiTextureTransform::MAYA_TRANSFORM;

    m_bMatrixDirty = true;
}
//---------------------------------------------------------------------------
bool NiTextureTransform::operator==(const NiTextureTransform &kXform) const
{
    return 
        m_fRotate == kXform.m_fRotate &&
        m_kTranslate == kXform.m_kTranslate &&
        m_kScale == kXform.m_kScale &&
        m_eMethod == kXform.m_eMethod &&
        m_kCenter == kXform.m_kCenter;
}
//---------------------------------------------------------------------------
bool NiTextureTransform::operator!=(const NiTextureTransform &kXform) const
{
    return 
        m_fRotate != kXform.m_fRotate ||
        m_kTranslate != kXform.m_kTranslate ||
        m_kScale != kXform.m_kScale ||
        m_eMethod != kXform.m_eMethod ||
        m_kCenter != kXform.m_kCenter;
}
//---------------------------------------------------------------------------
void NiTextureTransform::LoadBinary(NiStream& kStream)
{
    m_kTranslate.LoadBinary(kStream);
    m_kScale.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRotate);

    NiStreamLoadEnum(kStream, m_eMethod);

    m_kCenter.LoadBinary(kStream);

    m_bMatrixDirty = true;
}
//---------------------------------------------------------------------------
void NiTextureTransform::SaveBinary(NiStream& kStream)
{
    m_kTranslate.SaveBinary(kStream);
    m_kScale.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRotate);

    NiStreamSaveEnum(kStream, m_eMethod);

    m_kCenter.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTextureTransform::UpdateMatrix() const
{
    float fCos;
    float fSin;
    NiSinCos(m_fRotate, fSin, fCos);
    
    switch (m_eMethod)
    {
    case MAX_TRANSFORM:
    
        // This is the matrix solution use to create the
        // mathmatical solution. The commented out code was used
        // to validate the mathmatical solutions. When we are confident
        // of the solution this can be removed.
        
        /*
        NiMatrix3 kRotMatrix;
        kRotMatrix.MakeZRotation(m_fRotate);

        NiMatrix3 kScale;
        kScale.SetRow(0, m_kScale.x, 0.0f, 0.0f);
        kScale.SetRow(1, 0.0f, m_kScale.y, 0.0f);
        kScale.SetRow(2, 0.0f, 0.0f, 1.0f);

        NiMatrix3 kTranslate;
        kTranslate.SetRow(0, 1.0f, 0.0f, -m_kTranslate.x);
        kTranslate.SetRow(1, 0.0f, 1.0f, m_kTranslate.y);
        kTranslate.SetRow(2, 0.0f, 0.0f, 1.0f);

        NiMatrix3 kCenter;
        kCenter.SetRow(0, 1.0f, 0.0f, m_kCenter.x);
        kCenter.SetRow(1, 0.0f, 1.0f, m_kCenter.y);
        kCenter.SetRow(2, 0.0f, 0.0f, 1.0f);


        NiMatrix3 kBack;
        kBack.SetRow(0, 1.0f, 0.0f, -m_kCenter.x);
        kBack.SetRow(1, 0.0f, 1.0f, -m_kCenter.y);
        kBack.SetRow(2, 0.0f, 0.0f, 1.0f);

        NiMatrix3 kFinal = kCenter * kScale * kRotMatrix * kTranslate * kBack ;
        */   

        // This is the behavior in Max
        m_kMatrix.SetCol(0, m_kScale.x * fCos, m_kScale.y * -fSin, 0.0f);
        m_kMatrix.SetCol(1, m_kScale.x * fSin, m_kScale.y * fCos, 0.0f);
        m_kMatrix.SetCol(2,
            m_kCenter.x + m_kScale.x *
            (fCos * (-m_kCenter.x - m_kTranslate.x) + 
            fSin * (-m_kCenter.y + m_kTranslate.y)),
            m_kCenter.y + m_kScale.y * 
            (-fSin * (-m_kCenter.x - m_kTranslate.x) + 
            fCos * (-m_kCenter.y + m_kTranslate.y)), 1.0f);

        break;
    case MAYA_TRANSFORM:
        // This is the matrix solution use to create the
        // mathmatical solution. The commented out code was used
        // to validate the mathmatical solutions. When we are confident
        // of the solution this can be removed.

        /*
            NiMatrix3 kRotMatrix;
            kRotMatrix.MakeZRotation(-m_fRotate);

            NiMatrix3 kScale;
            kScale.SetRow(0, m_kScale.x, 0.0f, 0.0f);
            kScale.SetRow(1, 0.0f, m_kScale.y, 0.0f);
            kScale.SetRow(2, 0.0f, 0.0f, 1.0f);

            NiMatrix3 kTranslate;
            kTranslate.SetRow(0, 1.0f, 0.0f, m_kTranslate.x);
            kTranslate.SetRow(1, 0.0f, 1.0f, m_kTranslate.y);
            kTranslate.SetRow(2, 0.0f, 0.0f, 1.0f);

            NiMatrix3 kCenter;
            kCenter.SetRow(0, 1.0f, 0.0f, m_kCenter.x);
            kCenter.SetRow(1, 0.0f, 1.0f, m_kCenter.y);
            kCenter.SetRow(2, 0.0f, 0.0f, 1.0f);


            NiMatrix3 kBack;
            kBack.SetRow(0, 1.0f, 0.0f, -m_kCenter.x);
            kBack.SetRow(1, 0.0f, 1.0f, -m_kCenter.y);
            kBack.SetRow(2, 0.0f, 0.0f, 1.0f);

            NiMatrix3 kFromMaya;
            kFromMaya.SetRow(0, 1.0f,  0.0f, 0.0f);
            kFromMaya.SetRow(1, 0.0f, -1.0f, 1.0f);
            kFromMaya.SetRow(2, 0.0f,  0.0f,  1.0f);

            m_kMatrix =  kCenter * kRotMatrix * kBack * kFromMaya * 
                kTranslate * kScale;
        */



        // Mathmatical solution
        m_kMatrix.SetCol(0, fCos * m_kScale.x, -fSin * m_kScale.x, 0.0f);
        m_kMatrix.SetCol(1, -fSin * m_kScale.y, -fCos * m_kScale.y, 0.0f);
        m_kMatrix.SetCol(2, ((m_kTranslate.x - m_kCenter.x) * fCos) + 
            ((-m_kTranslate.y - m_kCenter.y + 1) * fSin) + m_kCenter.x, 
            ((-m_kTranslate.x + m_kCenter.x) * fSin) + 
            ((-m_kTranslate.y - m_kCenter.y + 1) * fCos) + m_kCenter.y, 1.0f);
        break;

    case MAYA_DEPRECATED_TRANSFORM:
        // This is the matrix solution use to create the
        // mathmatical solution. The commented out code was used
        // to validate the mathmatical solutions. When we are confident
        // of the solution this can be removed.

        
         /*   
            NiMatrix3 kRotMatrix;
            kRotMatrix.MakeZRotation(-m_fRotate);

            NiMatrix3 kScale;
            kScale.SetRow(0, m_kScale.x, 0.0f, 0.0f);
            kScale.SetRow(1, 0.0f, m_kScale.y, 0.0f);
            kScale.SetRow(2, 0.0f, 0.0f, 1.0f);

            NiMatrix3 kTranslate;
            kTranslate.SetRow(0, 1.0f, 0.0f, m_kTranslate.x);
            kTranslate.SetRow(1, 0.0f, 1.0f, m_kTranslate.y);
            kTranslate.SetRow(2, 0.0f, 0.0f, 1.0f);

            NiMatrix3 kCenter;
            kCenter.SetRow(0, 1.0f, 0.0f, m_kCenter.x);
            kCenter.SetRow(1, 0.0f, 1.0f, m_kCenter.y);
            kCenter.SetRow(2, 0.0f, 0.0f, 1.0f);


            NiMatrix3 kBack;
            kBack.SetRow(0, 1.0f, 0.0f, -m_kCenter.x);
            kBack.SetRow(1, 0.0f, 1.0f, -m_kCenter.y);
            kBack.SetRow(2, 0.0f, 0.0f, 1.0f);

            m_kMatrix = kCenter * kRotMatrix * kBack * kTranslate * kScale;
        */


        // Mathmatical solution
        m_kMatrix.SetCol(0, fCos * m_kScale.x, fSin * m_kScale.x, 0.0f);
        m_kMatrix.SetCol(1, -fSin * m_kScale.y, fCos * m_kScale.y, 0.0f);
        m_kMatrix.SetCol(2, ((m_kTranslate.x - m_kCenter.x) * fCos) + 
            ((m_kTranslate.y - m_kCenter.y) * -fSin) + m_kCenter.x, 
            ((m_kTranslate.x - m_kCenter.x) * fSin) + 
            ((m_kTranslate.y - m_kCenter.y) * fCos) + m_kCenter.y, 1.0f);
        break;
    default:
        NIASSERT(false); // you should never get here
    }

    m_bMatrixDirty = false;
}
//---------------------------------------------------------------------------

