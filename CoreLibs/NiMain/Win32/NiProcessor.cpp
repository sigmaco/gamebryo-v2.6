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

#include <NiSystem.h>
#include "NiProcessor.h"
#include "NiTransform.h"

void NiTransformVectorsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, NiMatrix3* pMatrix);
void NiTransformPointsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, const NiTransform *pXform);

NiProcessorSpecificCode::VectorTransformFunc 
    NiProcessorSpecificCode::ms_pVectorsTransform 
        = NiTransformVectorsPentium;
NiProcessorSpecificCode::PointTransformFunc 
    NiProcessorSpecificCode::ms_pPointsTransform 
        = NiTransformPointsPentium;

//#pragma optimize("w",on)

//---------------------------------------------------------------------------
// Assembly version - appears to be about 15-20% faster than the tuned C++
// code below (on a Pentium).  Change the above if to 0 if this code has 
// any problems (note that there is still ~1 stall cycle per loop)
void NiTransformVectorsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, NiMatrix3* pMatrix)
{
    // WARNING.  The assignment 'mov ecx, pMatrix' is valid because NiMatrix3
    // on the PC stores the matrix in row-major order as float[3][3].  Any
    // change to the data representation in NiMatrix3 could invalidate this
    // code.

__asm
{
    mov edx, uiVerts;
    cmp edx, 0;
    jnz NIASMVECLOOPSETUP;
    jmp NIASMVECEND;
    
NIASMVECLOOPSETUP:
    mov eax, pModel;
    mov ebx, pWorld;
    mov ecx, pMatrix;

NIASMVECLOOPTOP:
    fld DWORD PTR [eax];          mX        1        1
    fmul DWORD PTR [ecx];         mX*r00    2        2
    fld DWORD PTR [eax+4];        mY        3        3  2
    fmul DWORD PTR [ecx+4];       mY*r01    4        4  2
    fld DWORD PTR [eax+8];        mZ        5        5  4  2
    fmul DWORD PTR [ecx+8];       mZ*r02    6        6  4  2
    fxch;                                   7        4  6  2
    faddp ST(2),ST(0);            X0+Y0     8        6  8
    fld DWORD PTR [eax];          mX        9        9  6  8
    fmul DWORD PTR [ecx+12];      mX*r10    10      10  6  8
    fxch ST(2);                             11       8  6 10
    faddp ST(1),ST(0);            X0+Y0+Z0  12      12 10
    fxch;                                           10 12
    fld DWORD PTR [eax+4];        mY        13      13 10 12 
    fmul DWORD PTR [ecx+16];      mY*r11    14      14 10 12 
    fld DWORD PTR [eax+8];        mZ        15      15 14 10 12 
    fmul DWORD PTR [ecx+20];      mZ*r12    16      16 14 10 12 
    fxch ST(3);                             17      12 14 10 16 
    fstp DWORD PTR [ebx];         X0+Y0+Z0  18      14 10 16 
    faddp ST(1),ST(0);            X1+Y1     19      19 16 
    fld DWORD PTR [eax];          mX        20      20 19 16 
    fmul DWORD PTR [ecx+24];      mX*r20    21      21 19 16
    fxch ST(2);                             22      16 19 21
    faddp ST(1),ST(0);            X1+Y1+Z1  23      23 21
    fld DWORD PTR [eax+4];        mY        24      24 23 21 
    fmul DWORD PTR [ecx+28];      mY*r21    25      25 23 21 
    fld DWORD PTR [eax+8];        mZ        26      26 25 23 21 
    fmul DWORD PTR [ecx+32];      mZ*r22    27      27 25 23 21 
    fxch;                                   28      25 27 23 21 
    faddp ST(3),ST(0);            X2+Y2     29      27 23 29 
    fxch;                                   30      23 27 29 
    fstp DWORD PTR [ebx+4];       X1+Y1+Z1  31      27 29 
    fadd;                         X2+Y2+Y2  32      32 
    add eax, 12;
    add ebx, 12;
    fstp DWORD PTR [ebx-4];       X2+Y2+Y2  33      EMPTY
    dec edx;
    jnz NIASMVECLOOPTOP;
NIASMVECEND:
}
}
//---------------------------------------------------------------------------
void NiTransformPointsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, const NiTransform *pXform)
{
    float fTX = pXform->m_Translate.x;
    float fTY = pXform->m_Translate.y;
    float fTZ = pXform->m_Translate.z;
    float fWS = pXform->m_fScale;

    float pScaledMatData[9];
    float *pScaledMat = pScaledMatData;

    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) &pXform->m_Rotate;

    pScaledMat[0] = fWS * pFMatrix[0];
    pScaledMat[1] = fWS * pFMatrix[1];
    pScaledMat[2] = fWS * pFMatrix[2];
    pScaledMat[3] = fWS * pFMatrix[3];
    pScaledMat[4] = fWS * pFMatrix[4];
    pScaledMat[5] = fWS * pFMatrix[5];
    pScaledMat[6] = fWS * pFMatrix[6];
    pScaledMat[7] = fWS * pFMatrix[7];
    pScaledMat[8] = fWS * pFMatrix[8];

__asm
{
    mov edx, uiVerts;
    cmp edx, 0;
    jnz NIASMPTLOOPSETUP;
    jmp NIASMPTEND;
    
NIASMPTLOOPSETUP:

    mov eax, pModel;
    mov ebx, pWorld;
    mov ecx, pScaledMat;

NIASMPTLOOPTOP:
    fld DWORD PTR [eax];          mX        1        1
    fmul DWORD PTR [ecx];         mX*r00    2        2
    fld DWORD PTR [eax+4];        mY        3        3  2        
    fmul DWORD PTR [ecx+4];       mY*r01    4        4  2
    fld DWORD PTR [eax+8];        mZ        5        5  4  2
    fmul DWORD PTR [ecx+8];       mZ*r02    6        6  4  2
    fxch;                                            4  6  2
    faddp ST(2),ST(0);            X0+Y0     7        6  7
    fld DWORD PTR [eax];          mX        8        8  6  7
    fmul DWORD PTR [ecx+12];      mX*r10    9        9  6  7
    fxch ST(2);                                      7  6  9
    faddp ST(1),ST(0);            X0+Y0+Z0  10      10  9
    fxch;                                            9 10
    fld DWORD PTR [eax+4];        mY        11      11  9 10
    fmul DWORD PTR [ecx+16];      mY*r11    12      12  9 10
    fld DWORD PTR [eax+8];        mZ        13      13 12  9 10
    fmul DWORD PTR [ecx+20];      mZ*r12    14      14 12  9 10
    fxch;                                           12 14  9 10
    faddp ST(2),ST(0);            X1+Y1     15      14 15 10
    fxch ST(2);                                     10 15 14
    fadd fTX;                     R0+TX     16      16 15 14
    fld DWORD PTR [ecx+24];       r20       17      17 16 15 14
    fxch ST(2);                                     15 16 17 14
    faddp ST(3),ST(0);            X1+Y1+Z1  18      16 17 18
    fxch;                                           17 16 18
    fmul DWORD PTR [eax];         mX*r20    19      19 16 18
    fld DWORD PTR [ecx+28];       r21       20      20 19 16 18
    fld DWORD PTR [ecx+32];       r22       21      21 20 19 16 18
    fxch;                                           20 21 19 16 18
    fmul DWORD PTR [eax+4];       mY*r21    22      22 21 19 16 18
    fxch ST(4);                                     18 21 19 16 22
    fadd fTY;                     R1+TY     23      23 21 19 16 22
    fxch;                                           21 23 19 16 22
    fmul DWORD PTR [eax+8];       mZ*r22    24      24 23 19 16 22
    fxch ST(4);                                     22 23 19 16 24
    faddp ST(2),ST(0);            X2+Y2     25      23 25 16 24
    fxch ST(2);                                     16 25 23 24
    fstp DWORD PTR [ebx];         WX        26-27   25 23 24
    faddp ST(2),ST(0);            X2+Y2+Z2  28      23 28
    fstp DWORD PTR [ebx+4];       WY        29-30   28      
    fadd fTZ;                     R2+TZ     31      31
    add eax, 12;                            32      31
    add ebx, 12;                            33      31
    fstp DWORD PTR [ebx-4];       WZ        34-35   EMPTY 

    dec edx;
    jnz NIASMPTLOOPTOP;
NIASMPTEND:
}
}
//#pragma optimize("",off)

//---------------------------------------------------------------------------
void NiProcessorSpecificCode::TransformPointsBasic (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, const NiTransform *pXform)
{
    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) &pXform->m_Rotate;

    for (NiUInt32 i = 0; i < uiVerts; i++, pModel += 3, pWorld += 3)
    {
        // wn = wr*n
        *pWorld = (pFMatrix[0]*(*pModel) +
                  pFMatrix[1]*(*(pModel+1)) +
                  pFMatrix[2]*(*(pModel+2)))*pXform->m_fScale + 
                  pXform->m_Translate.x;
        *(pWorld+1) = (pFMatrix[3]*(*pModel) +
                      pFMatrix[4]*(*(pModel+1)) +
                      pFMatrix[5]*(*(pModel+2)))*pXform->m_fScale + 
                      pXform->m_Translate.y;
        *(pWorld+2) = (pFMatrix[6]*(*pModel) +
                      pFMatrix[7]*(*(pModel+1)) +
                      pFMatrix[8]*(*(pModel+2)))*pXform->m_fScale + 
                      pXform->m_Translate.z;
    }
}
//---------------------------------------------------------------------------
void NiProcessorSpecificCode::TransformVectorsBasic (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, NiMatrix3* pMatrix)
{
    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) pMatrix;

    for (NiUInt32 i = 0; i < uiVerts; i++, pModel+=3, pWorld+=3)
    {
        // wn = wr*n
        *pWorld = pFMatrix[0]*(*pModel)
                 +pFMatrix[1]*(*(pModel+1))
                 +pFMatrix[2]*(*(pModel+2));
        *(pWorld+1) = pFMatrix[3]*(*pModel)
                     +pFMatrix[4]*(*(pModel+1))
                     +pFMatrix[5]*(*(pModel+2));
        *(pWorld+2) = pFMatrix[6]*(*pModel)
                     +pFMatrix[7]*(*(pModel+1))
                     +pFMatrix[8]*(*(pModel+2));
    }
}
//---------------------------------------------------------------------------
