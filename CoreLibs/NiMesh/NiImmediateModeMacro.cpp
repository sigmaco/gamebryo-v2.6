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
#include "NiMeshPCH.h"

#include "NiImmediateModeMacro.h"
#include "NiSkinningMeshModifier.h"

#include "NiCamera.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"
#include "NiDirectionalLight.h"

// Include all "standard" objects like cameras, point lights, etc... here
// as static data rather than loading them as external .nif files.

static NiPoint3 g_akWireCameraVerts[] =
{

    NiPoint3(-0.86956525f, 0.17391305f, 0.086956523f),
    NiPoint3(-0.86956525f, -0.17391305f, 0.086956523f),
    NiPoint3(-0.26086959f, -0.17391305f, 0.086956523f),
    NiPoint3(-0.26086959f, 0.17391305f, 0.086956523f),
    NiPoint3(-0.86956525f, 0.17391305f, 0.086956523f),
    NiPoint3(-0.34782609f, 0.17391305f, 0.043478262f),
    NiPoint3(-0.31259045f, 0.17676125f, 0.043478265f),
    NiPoint3(-0.27915519f, 0.18500622f, 0.043478262f),
    NiPoint3(-0.24797004f, 0.19819836f, 0.043478262f),
    NiPoint3(-0.21948439f, 0.21588819f, 0.043478262f),
    NiPoint3(-0.19414788f, 0.23762614f, 0.043478262f),
    NiPoint3(-0.17240991f, 0.26296270f, 0.043478265f),
    NiPoint3(-0.15472014f, 0.29144832f, 0.043478262f),
    NiPoint3(-0.14152797f, 0.32263353f, 0.043478262f),
    NiPoint3(-0.13328303f, 0.35606867f, 0.043478265f),
    NiPoint3(-0.13043480f, 0.39130434f, 0.043478262f),
    NiPoint3(-0.13328302f, 0.42653999f, 0.043478265f),
    NiPoint3(-0.14152794f, 0.45997524f, 0.043478262f),
    NiPoint3(-0.15472014f, 0.49116039f, 0.043478262f),
    NiPoint3(-0.17240986f, 0.51964599f, 0.043478262f),
    NiPoint3(-0.19414788f, 0.54498255f, 0.043478262f),
    NiPoint3(-0.21948448f, 0.56672055f, 0.043478265f),
    NiPoint3(-0.24797010f, 0.58441037f, 0.043478262f),
    NiPoint3(-0.27915525f, 0.59760249f, 0.043478262f),
    NiPoint3(-0.31259045f, 0.60584742f, 0.043478265f),
    NiPoint3(-0.34782612f, 0.60869569f, 0.043478262f),
    NiPoint3(-0.38306180f, 0.60584742f, 0.043478265f),
    NiPoint3(-0.41649696f, 0.59760255f, 0.043478262f),
    NiPoint3(-0.44768217f, 0.58441037f, 0.043478262f),
    NiPoint3(-0.47616780f, 0.56672060f, 0.043478262f),
    NiPoint3(-0.50150430f, 0.54498255f, 0.043478262f),
    NiPoint3(-0.52324229f, 0.51964593f, 0.043478265f),
    NiPoint3(-0.54093212f, 0.49116039f, 0.043478262f),
    NiPoint3(-0.55412424f, 0.45997518f, 0.043478262f),
    NiPoint3(-0.56236923f, 0.42653999f, 0.043478265f),
    NiPoint3(-0.56521738f, 0.39130431f, 0.043478262f),
    NiPoint3(-0.56521738f, 0.39130437f, 0.043478258f),
    NiPoint3(-0.56806564f, 0.42654002f, 0.043478258f),
    NiPoint3(-0.57631058f, 0.45997524f, 0.043478251f),
    NiPoint3(-0.58950275f, 0.49116039f, 0.043478247f),
    NiPoint3(-0.60719258f, 0.51964611f, 0.043478239f),
    NiPoint3(-0.62893051f, 0.54498261f, 0.043478247f),
    NiPoint3(-0.65426707f, 0.56672055f, 0.043478251f),
    NiPoint3(-0.68275273f, 0.58441037f, 0.043478251f),
    NiPoint3(-0.71393794f, 0.59760249f, 0.043478258f),
    NiPoint3(-0.74737310f, 0.60584742f, 0.043478258f),
    NiPoint3(-0.78260875f, 0.60869569f, 0.043478258f),
    NiPoint3(-0.81784445f, 0.60584742f, 0.043478258f),
    NiPoint3(-0.85127956f, 0.59760255f, 0.043478258f),
    NiPoint3(-0.88246483f, 0.58441037f, 0.043478262f),
    NiPoint3(-0.91095036f, 0.56672060f, 0.043478262f),
    NiPoint3(-0.93628699f, 0.54498255f, 0.043478262f),
    NiPoint3(-0.95802498f, 0.51964593f, 0.043478265f),
    NiPoint3(-0.97571480f, 0.49116039f, 0.043478269f),
    NiPoint3(-0.98890692f, 0.45997518f, 0.043478265f),
    NiPoint3(-0.99715185f, 0.42653999f, 0.043478269f),
    NiPoint3(-1.0000001f, 0.39130431f, 0.043478269f),
    NiPoint3(-0.99715185f, 0.35606864f, 0.043478277f),
    NiPoint3(-0.98890704f, 0.32263348f, 0.043478277f),
    NiPoint3(-0.97571468f, 0.29144832f, 0.043478269f),
    NiPoint3(-0.95802498f, 0.26296273f, 0.043478277f),
    NiPoint3(-0.93628699f, 0.23762611f, 0.043478280f),
    NiPoint3(-0.91095036f, 0.21588813f, 0.043478280f),
    NiPoint3(-0.88246471f, 0.19819833f, 0.043478277f),
    NiPoint3(-0.85127956f, 0.18500619f, 0.043478269f),
    NiPoint3(-0.81784439f, 0.17676125f, 0.043478277f),
    NiPoint3(-0.78260869f, 0.17391305f, 0.043478269f),
    NiPoint3(-0.56521738f, 0.39130437f, -0.043478280f),
    NiPoint3(-0.56806564f, 0.42654002f, -0.043478280f),
    NiPoint3(-0.57631058f, 0.45997524f, -0.043478291f),
    NiPoint3(-0.58950275f, 0.49116039f, -0.043478291f),
    NiPoint3(-0.60719258f, 0.51964611f, -0.043478291f),
    NiPoint3(-0.62893051f, 0.54498261f, -0.043478280f),
    NiPoint3(-0.65426707f, 0.56672055f, -0.043478280f),
    NiPoint3(-0.68275273f, 0.58441037f, -0.043478291f),
    NiPoint3(-0.71393794f, 0.59760249f, -0.043478280f),
    NiPoint3(-0.74737310f, 0.60584742f, -0.043478280f),
    NiPoint3(-0.78260875f, 0.60869569f, -0.043478280f),
    NiPoint3(-0.81784445f, 0.60584742f, -0.043478265f),
    NiPoint3(-0.85127956f, 0.59760255f, -0.043478291f),
    NiPoint3(-0.88246483f, 0.58441037f, -0.043478254f),
    NiPoint3(-0.91095036f, 0.56672060f, -0.043478280f),
    NiPoint3(-0.93628699f, 0.54498255f, -0.043478265f),
    NiPoint3(-0.95802498f, 0.51964593f, -0.043478265f),
    NiPoint3(-0.97571480f, 0.49116039f, -0.043478265f),
    NiPoint3(-0.98890692f, 0.45997518f, -0.043478265f),
    NiPoint3(-0.99715185f, 0.42653999f, -0.043478254f),
    NiPoint3(-1.0000001f, 0.39130431f, -0.043478254f),
    NiPoint3(-0.99715185f, 0.35606864f, -0.043478239f),
    NiPoint3(-0.98890704f, 0.32263348f, -0.043478265f),
    NiPoint3(-0.97571468f, 0.29144832f, -0.043478254f),
    NiPoint3(-0.95802498f, 0.26296273f, -0.043478265f),
    NiPoint3(-0.93628699f, 0.23762611f, -0.043478254f),
    NiPoint3(-0.91095036f, 0.21588813f, -0.043478239f),
    NiPoint3(-0.88246471f, 0.19819833f, -0.043478254f),
    NiPoint3(-0.85127956f, 0.18500619f, -0.043478254f),
    NiPoint3(-0.81784439f, 0.17676125f, -0.043478254f),
    NiPoint3(-0.78260869f, 0.17391305f, -0.043478254f),
    NiPoint3(-0.34782609f, 0.17391305f, -0.043478265f),
    NiPoint3(-0.31259045f, 0.17676125f, -0.043478254f),
    NiPoint3(-0.27915519f, 0.18500622f, -0.043478280f),
    NiPoint3(-0.24797004f, 0.19819836f, -0.043478254f),
    NiPoint3(-0.21948439f, 0.21588819f, -0.043478280f),
    NiPoint3(-0.19414788f, 0.23762614f, -0.043478265f),
    NiPoint3(-0.17240991f, 0.26296270f, -0.043478265f),
    NiPoint3(-0.15472014f, 0.29144832f, -0.043478265f),
    NiPoint3(-0.14152797f, 0.32263353f, -0.043478265f),
    NiPoint3(-0.13328303f, 0.35606867f, -0.043478254f),
    NiPoint3(-0.13043480f, 0.39130434f, -0.043478265f),
    NiPoint3(-0.13328302f, 0.42653999f, -0.043478254f),
    NiPoint3(-0.14152794f, 0.45997524f, -0.043478280f),
    NiPoint3(-0.15472014f, 0.49116039f, -0.043478254f),
    NiPoint3(-0.17240986f, 0.51964599f, -0.043478280f),
    NiPoint3(-0.19414788f, 0.54498255f, -0.043478265f),
    NiPoint3(-0.21948448f, 0.56672055f, -0.043478265f),
    NiPoint3(-0.24797010f, 0.58441037f, -0.043478265f),
    NiPoint3(-0.27915525f, 0.59760249f, -0.043478265f),
    NiPoint3(-0.31259045f, 0.60584742f, -0.043478254f),
    NiPoint3(-0.34782612f, 0.60869569f, -0.043478265f),
    NiPoint3(-0.38306180f, 0.60584742f, -0.043478254f),
    NiPoint3(-0.41649696f, 0.59760255f, -0.043478280f),
    NiPoint3(-0.44768217f, 0.58441037f, -0.043478254f),
    NiPoint3(-0.47616780f, 0.56672060f, -0.043478280f),
    NiPoint3(-0.50150430f, 0.54498255f, -0.043478265f),
    NiPoint3(-0.52324229f, 0.51964593f, -0.043478265f),
    NiPoint3(-0.54093212f, 0.49116039f, -0.043478265f),
    NiPoint3(-0.55412424f, 0.45997518f, -0.043478265f),
    NiPoint3(-0.56236923f, 0.42653999f, -0.043478254f),
    NiPoint3(-0.56521738f, 0.39130431f, -0.043478265f),
    NiPoint3(0.00000000f, 0.13043478f, -0.13043480f),
    NiPoint3(-5.1830128e-008f, 0.13043478f, 0.13043480f),
    NiPoint3(0.00000000f, -0.13043480f, 0.13043480f),
    NiPoint3(5.1830128e-008f, -0.13043480f, -0.13043480f),
    NiPoint3(0.00000000f, 0.13043478f, -0.13043480f),
    NiPoint3(-0.86956525f, 0.17391305f, -0.086956523f),
    NiPoint3(-0.86956525f, -0.17391306f, -0.086956523f),
    NiPoint3(-0.26086959f, -0.17391306f, -0.086956523f),
    NiPoint3(-0.26086959f, 0.17391305f, -0.086956523f),
    NiPoint3(-0.86956525f, 0.17391305f, -0.086956523f),
    NiPoint3(-0.26086956f, 0.17391305f, -0.086956523f),
    NiPoint3(-0.26086962f, 0.17391305f, 0.086956523f),
    NiPoint3(-0.86956525f, 0.17391305f, -0.086956523f),
    NiPoint3(-0.86956525f, 0.17391305f, 0.086956523f),
    NiPoint3(-0.86956525f, -0.17391305f, -0.086956523f),
    NiPoint3(-0.86956525f, -0.17391305f, 0.086956523f),
    NiPoint3(-0.26086962f, -0.17391305f, -0.086956523f),
    NiPoint3(-0.26086965f, -0.17391305f, 0.086956523f),
    NiPoint3(0.00000000f, -0.13043480f, 0.13043480f),
    NiPoint3(-0.26086959f, -0.086956523f, 0.043478262f),
    NiPoint3(-0.26086959f, 0.086956523f, 0.043478262f),
    NiPoint3(0.00000000f, 0.13043480f, 0.13043480f),
    NiPoint3(0.00000000f, -0.13043480f, -0.13043480f),
    NiPoint3(-0.26086959f, -0.086956531f, -0.043478265f),
    NiPoint3(-0.26086959f, 0.086956523f, -0.043478265f),
    NiPoint3(0.00000000f, 0.13043480f, -0.13043480f),
    NiPoint3(-0.26086959f, 0.086956516f, 0.043478262f),
    NiPoint3(-0.26086959f, 0.086956516f, -0.043478265f),
    NiPoint3(-0.26086959f, -0.086956538f, 0.043478262f),
    NiPoint3(-0.26086959f, -0.086956538f, -0.043478265f),
    NiPoint3(-0.78260875f, 0.17391305f, 0.043478262f),
    NiPoint3(-0.34782609f, 0.17391305f, 0.043478262f),
    NiPoint3(-0.78260875f, 0.17391305f, -0.043478265f),
    NiPoint3(-0.34782609f, 0.17391305f, -0.043478265f),
    NiPoint3(-0.56521738f, 0.39130437f, -0.043478265f),
    NiPoint3(-0.56521738f, 0.39130437f, 0.043478262f)
};

static unsigned int g_auiWireCameraIndices[] =
{
    0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 
    12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 
    21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 
    30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 36, 37, 37, 38, 38, 39, 39, 40, 
    40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 
    49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 
    58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 67, 68, 
    68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 
    77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 
    86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 
    95, 96, 96, 97, 98, 99, 99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 
    104, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 
    111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 
    118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 
    125, 126, 126, 127, 127, 128, 129, 130, 130, 131, 131, 132, 132, 133, 
    134, 135, 135, 136, 136, 137, 137, 138, 139, 140, 141, 142, 143, 144, 
    145, 146, 147, 148, 148, 149, 149, 150, 151, 152, 152, 153, 153, 154, 
    155, 156, 157, 158, 159, 160, 161, 162, 163, 164
};


static NiPoint3 g_akWirePointLightVerts[] =
{
    NiPoint3(-3.9733128e-009f, 6.9545365e-009f, 0.25000000f),
    NiPoint3(-3.8301393e-009f, 0.040521018f, 0.24672458f),
    NiPoint3(-3.4156815e-009f, 0.078971490f, 0.23724289f),
    NiPoint3(-2.7525364e-009f, 0.11483444f, 0.22207190f),
    NiPoint3(-1.8633028e-009f, 0.14759293f, 0.20172863f),
    NiPoint3(-7.7057694e-010f, 0.17672998f, 0.17672995f),
    NiPoint3(5.0304427e-010f, 0.20172861f, 0.14759289f),
    NiPoint3(1.9349629e-009f, 0.22207192f, 0.11483441f),
    NiPoint3(3.5025820e-009f, 0.23724289f, 0.078971460f),
    NiPoint3(5.1833045e-009f, 0.24672458f, 0.040521011f),
    NiPoint3(6.9545343e-009f, 0.25000003f, -1.0927847e-008f),
    NiPoint3(8.7257650e-009f, 0.24672461f, -0.040521026f),
    NiPoint3(1.0406488e-008f, 0.23724292f, -0.078971490f),
    NiPoint3(1.1974107e-008f, 0.22207192f, -0.11483444f),
    NiPoint3(1.3406027e-008f, 0.20172863f, -0.14759293f),
    NiPoint3(1.4679646e-008f, 0.17672996f, -0.17672996f),
    NiPoint3(1.5772372e-008f, 0.14759289f, -0.20172861f),
    NiPoint3(1.6661605e-008f, 0.11483441f, -0.22207190f),
    NiPoint3(1.7324750e-008f, 0.078971460f, -0.23724288f),
    NiPoint3(1.7739207e-008f, 0.040521007f, -0.24672455f),
    NiPoint3(1.7882382e-008f, -1.4901161e-008f, -0.25000000f),
    NiPoint3(1.7739207e-008f, -0.040521029f, -0.24672455f),
    NiPoint3(1.7324750e-008f, -0.078971490f, -0.23724286f),
    NiPoint3(1.6661604e-008f, -0.11483444f, -0.22207187f),
    NiPoint3(1.5772374e-008f, -0.14759295f, -0.20172861f),
    NiPoint3(1.4679644e-008f, -0.17672998f, -0.17672993f),
    NiPoint3(1.3406024e-008f, -0.20172861f, -0.14759287f),
    NiPoint3(1.1974105e-008f, -0.22207190f, -0.11483439f),
    NiPoint3(1.0406485e-008f, -0.23724288f, -0.078971438f),
    NiPoint3(8.7257632e-009f, -0.24672455f, -0.040520988f),
    NiPoint3(6.9545329e-009f, -0.25000000f, 3.2783543e-008f),
    NiPoint3(5.1833036e-009f, -0.24672455f, 0.040521041f),
    NiPoint3(3.5025802e-009f, -0.23724286f, 0.078971505f),
    NiPoint3(1.9349606e-009f, -0.22207187f, 0.11483446f),
    NiPoint3(5.0304338e-010f, -0.20172863f, 0.14759293f),
    NiPoint3(-7.7057871e-010f, -0.17672995f, 0.17672998f),
    NiPoint3(-1.8633037e-009f, -0.14759289f, 0.20172861f),
    NiPoint3(-2.7525364e-009f, -0.11483442f, 0.22207190f),
    NiPoint3(-3.4156815e-009f, -0.078971468f, 0.23724288f),
    NiPoint3(-3.8301384e-009f, -0.040521014f, 0.24672455f),
    NiPoint3(-3.9733128e-009f, 6.9545365e-009f, 0.25000000f),
    NiPoint3(6.9545343e-009f, 0.49999997f, 3.0399234e-016f),
    NiPoint3(9.1401038e-009f, 0.34999999f, -0.050000001f),
    NiPoint3(4.7689648e-009f, 0.34999999f, 0.050000001f),
    NiPoint3(6.9545343e-009f, 0.49999997f, 3.0399234e-016f),
    NiPoint3(-6.9014217e-009f, 0.43301269f, 0.25000000f),
    NiPoint3(2.4090863e-010f, 0.32810888f, 0.13169874f),
    NiPoint3(-5.7301781e-009f, 0.27810887f, 0.21830128f),
    NiPoint3(-6.9014217e-009f, 0.43301269f, 0.25000000f),
    NiPoint3(-2.2900904e-008f, 0.24999997f, 0.43301275f),
    NiPoint3(-1.0958728e-008f, 0.21830124f, 0.27810892f),
    NiPoint3(-1.6929816e-008f, 0.13169870f, 0.32810894f),
    NiPoint3(-2.2900904e-008f, 0.24999997f, 0.43301275f),
    NiPoint3(-3.6756855e-008f, -4.4703484e-008f, 0.50000000f),
    NiPoint3(-2.1457870e-008f, 0.049999963f, 0.35000002f),
    NiPoint3(-2.5829010e-008f, -0.050000042f, 0.35000002f),
    NiPoint3(-3.6756855e-008f, -4.4703484e-008f, 0.50000000f),
    NiPoint3(6.9545329e-009f, -1.6885346e-008f, -0.50000000f),
    NiPoint3(4.7689639e-009f, -0.050000019f, -0.35000002f),
    NiPoint3(9.1401038e-009f, 0.049999982f, -0.35000002f),
    NiPoint3(6.9545329e-009f, -1.6885346e-008f, -0.50000000f),
    NiPoint3(-4.4756597e-008f, -0.25000009f, 0.43301272f),
    NiPoint3(-2.8443285e-008f, -0.13169880f, 0.32810891f),
    NiPoint3(-3.0043232e-008f, -0.21830134f, 0.27810889f),
    NiPoint3(-4.4756597e-008f, -0.25000009f, 0.43301272f),
    NiPoint3(-4.4756597e-008f, -0.43301275f, 0.25000006f),
    NiPoint3(-3.0043232e-008f, -0.27810892f, 0.21830131f),
    NiPoint3(-2.8443285e-008f, -0.32810894f, 0.13169877f),
    NiPoint3(-4.4756597e-008f, -0.43301275f, 0.25000006f),
    NiPoint3(-3.6756862e-008f, -0.50000000f, 7.5497908e-008f),
    NiPoint3(-2.5829010e-008f, -0.35000005f, 0.050000057f),
    NiPoint3(-2.1457877e-008f, -0.35000005f, -0.049999949f),
    NiPoint3(-3.6756862e-008f, -0.50000000f, 7.5497908e-008f),
    NiPoint3(-2.2900911e-008f, -0.43301281f, -0.24999991f),
    NiPoint3(-1.6929818e-008f, -0.32810894f, -0.13169865f),
    NiPoint3(-1.0958735e-008f, -0.27810898f, -0.21830121f),
    NiPoint3(-2.2900911e-008f, -0.43301281f, -0.24999991f),
    NiPoint3(-6.9014341e-009f, -0.25000015f, -0.43301263f),
    NiPoint3(-5.7301861e-009f, -0.21830140f, -0.27810884f),
    NiPoint3(2.4090152e-010f, -0.13169889f, -0.32810888f),
    NiPoint3(-6.9014341e-009f, -0.25000015f, -0.43301263f),
    NiPoint3(1.4954272e-008f, 0.24999994f, -0.43301275f),
    NiPoint3(1.1754379e-008f, 0.13169868f, -0.32810894f),
    NiPoint3(1.3354327e-008f, 0.21830124f, -0.27810892f),
    NiPoint3(1.4954272e-008f, 0.24999994f, -0.43301275f),
    NiPoint3(1.4954276e-008f, 0.43301269f, -0.25000012f),
    NiPoint3(1.3354327e-008f, 0.27810887f, -0.21830134f),
    NiPoint3(1.1754382e-008f, 0.32810888f, -0.13169882f),
    NiPoint3(1.4954276e-008f, 0.43301269f, -0.25000012f)
};

static unsigned int g_auiPointLightIndices[] =
{
    0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 
    12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 
    21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 
    30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 
    39, 39, 40, 41, 42, 42, 43, 43, 44, 45, 46, 46, 47, 47, 48, 49, 50, 50, 
    51, 51, 52, 53, 54, 54, 55, 55, 56, 57, 58, 58, 59, 59, 60, 61, 62, 62, 
    63, 63, 64, 65, 66, 66, 67, 67, 68, 69, 70, 70, 71, 71, 72, 73, 74, 74, 
    75, 75, 76, 77, 78, 78, 79, 79, 80, 81, 82, 82, 83, 83, 84, 85, 86, 86, 
    87, 87, 88
};

static NiPoint3 g_akWireDirectionalLightVerts[] =
{
    NiPoint3(1.9873647e-009f, 0.50000006f, -0.49999991f),
    NiPoint3(1.9874209e-009f, 0.50000006f, 0.50000018f),
    NiPoint3(1.9669467e-007f, -0.50000006f, 0.50000012f),
    NiPoint3(1.9669464e-007f, -0.50000006f, -0.49999997f),
    NiPoint3(1.9873647e-009f, 0.50000006f, -0.49999991f),
    NiPoint3(9.8900088e-008f, 0.22222224f, 0.22222236f),
    NiPoint3(-0.55555546f, 0.22222215f, 0.22222245f),
    NiPoint3(-0.55555540f, -0.22222231f, 0.22222242f),
    NiPoint3(1.6600939e-007f, -0.22222224f, 0.22222231f),
    NiPoint3(9.8900088e-008f, 0.22222224f, 0.22222236f),
    NiPoint3(1.0530346e-007f, 0.50000006f, 0.50000018f),
    NiPoint3(0.55555564f, 8.3886619e-008f, 8.8296153e-009f),
    NiPoint3(2.5629933e-007f, -0.50000000f, 0.50000012f),
    NiPoint3(3.2672688e-008f, 0.22222224f, -0.22222212f),
    NiPoint3(-0.55555552f, 0.22222215f, -0.22222202f),
    NiPoint3(-0.55555552f, -0.22222231f, -0.22222206f),
    NiPoint3(9.9781971e-008f, -0.22222224f, -0.22222215f),
    NiPoint3(3.2672688e-008f, 0.22222224f, -0.22222212f),
    NiPoint3(-5.7617292e-008f, 0.50000006f, -0.49999991f),
    NiPoint3(0.55555564f, 8.3886619e-008f, 8.8296153e-009f),
    NiPoint3(9.3378610e-008f, -0.50000000f, -0.49999997f),
    NiPoint3(-0.55555540f, -0.22222231f, 0.22222240f),
    NiPoint3(-0.55555552f, -0.22222233f, -0.22222207f),
    NiPoint3(-0.55555546f, 0.22222215f, 0.22222243f),
    NiPoint3(-0.55555558f, 0.22222213f, -0.22222203f),
    NiPoint3(1.6910029e-007f, -0.22222224f, 0.22222231f),
    NiPoint3(9.6691068e-008f, -0.22222224f, -0.22222215f),
    NiPoint3(1.0199099e-007f, 0.22222224f, 0.22222236f),
    NiPoint3(2.9581784e-008f, 0.22222224f, -0.22222212f)
};

static unsigned int g_auiDirectionalLightIndices[] =
{
    0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 
    14, 15, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28
};

static NiPoint3 g_akWireSpotLightVerts[] =
{
    NiPoint3(-2.0066963e-007f, 0.24999996f, 0.25000021f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.25000021f),
    NiPoint3(0.49999979f, -0.50000006f, 0.50000024f),
    NiPoint3(0.49999979f, 0.49999997f, 0.50000024f),
    NiPoint3(-2.0066963e-007f, 0.24999996f, 0.25000021f),
    NiPoint3(-0.50000018f, 0.24999999f, -0.24999979f),
    NiPoint3(-0.50000018f, -0.25000003f, -0.24999979f),
    NiPoint3(-0.50000018f, 0.24999996f, 0.25000021f),
    NiPoint3(-0.50000018f, -0.25000003f, 0.25000021f),
    NiPoint3(-2.0066963e-007f, 0.24999999f, -0.24999979f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, -0.24999979f),
    NiPoint3(0.49999979f, -0.50000000f, -0.49999982f),
    NiPoint3(0.49999979f, 0.49999997f, -0.49999982f),
    NiPoint3(-2.0066963e-007f, 0.24999999f, -0.24999979f),
    NiPoint3(0.49999979f, 0.49999994f, 0.50000024f),
    NiPoint3(0.49999979f, 0.49999997f, -0.49999982f),
    NiPoint3(0.49999979f, -0.50000006f, 0.50000024f),
    NiPoint3(0.49999979f, -0.50000000f, -0.49999982f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.050000194f, -0.25000003f, 0.25000018f),
    NiPoint3(-0.10000020f, -0.25000006f, 0.25000021f),
    NiPoint3(-0.15000018f, -0.25000000f, 0.25000021f),
    NiPoint3(-0.20000023f, -0.25000006f, 0.25000024f),
    NiPoint3(-0.25000018f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.30000019f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.35000017f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.40000021f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.45000014f, -0.25000000f, 0.25000021f),
    NiPoint3(-0.50000018f, -0.25000003f, 0.25000021f),
    NiPoint3(-0.50000012f, -0.25000003f, 0.20000018f),
    NiPoint3(-0.50000018f, -0.25000006f, 0.15000021f),
    NiPoint3(-0.50000018f, -0.25000000f, 0.10000020f),
    NiPoint3(-0.50000024f, -0.25000003f, 0.050000206f),
    NiPoint3(-0.50000018f, -0.25000003f, 2.0489097e-007f),
    NiPoint3(-0.50000018f, -0.25000000f, -0.049999807f),
    NiPoint3(-0.50000012f, -0.25000000f, -0.099999793f),
    NiPoint3(-0.50000018f, -0.25000000f, -0.14999980f),
    NiPoint3(-0.50000018f, -0.25000000f, -0.19999978f),
    NiPoint3(-0.50000018f, -0.25000000f, -0.24999979f),
    NiPoint3(-0.45000014f, -0.25000000f, -0.24999978f),
    NiPoint3(-0.40000018f, -0.25000003f, -0.24999982f),
    NiPoint3(-0.35000020f, -0.25000000f, -0.24999979f),
    NiPoint3(-0.30000022f, -0.25000000f, -0.24999982f),
    NiPoint3(-0.25000018f, -0.25000000f, -0.24999979f),
    NiPoint3(-0.20000018f, -0.25000000f, -0.24999979f),
    NiPoint3(-0.15000018f, -0.24999999f, -0.24999979f),
    NiPoint3(-0.10000020f, -0.25000000f, -0.24999979f),
    NiPoint3(-0.050000206f, -0.25000000f, -0.24999979f),
    NiPoint3(-2.0066963e-007f, -0.25000000f, -0.24999979f),
    NiPoint3(-2.0066962e-007f, -0.25000000f, -0.19999978f),
    NiPoint3(-2.0066965e-007f, -0.25000003f, -0.14999981f),
    NiPoint3(-2.0066962e-007f, -0.25000000f, -0.099999800f),
    NiPoint3(-2.0066965e-007f, -0.25000006f, -0.049999807f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 2.0116568e-007f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.050000217f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.10000020f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.15000021f),
    NiPoint3(-2.0066963e-007f, -0.25000000f, 0.20000018f),
    NiPoint3(-2.0066963e-007f, -0.25000003f, 0.25000021f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.050000194f, 0.24999991f, 0.25000018f),
    NiPoint3(-0.10000020f, 0.24999996f, 0.25000021f),
    NiPoint3(-0.15000018f, 0.24999996f, 0.25000021f),
    NiPoint3(-0.20000023f, 0.24999999f, 0.25000024f),
    NiPoint3(-0.25000018f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.30000019f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.35000017f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.40000021f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.45000014f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.50000018f, 0.24999994f, 0.25000021f),
    NiPoint3(-0.50000012f, 0.24999991f, 0.20000018f),
    NiPoint3(-0.50000018f, 0.24999996f, 0.15000021f),
    NiPoint3(-0.50000018f, 0.24999996f, 0.10000020f),
    NiPoint3(-0.50000024f, 0.24999999f, 0.050000206f),
    NiPoint3(-0.50000018f, 0.24999997f, 2.0489097e-007f),
    NiPoint3(-0.50000018f, 0.24999994f, -0.049999807f),
    NiPoint3(-0.50000012f, 0.24999996f, -0.099999793f),
    NiPoint3(-0.50000018f, 0.24999994f, -0.14999980f),
    NiPoint3(-0.50000018f, 0.24999994f, -0.19999978f),
    NiPoint3(-0.50000018f, 0.24999996f, -0.24999979f),
    NiPoint3(-0.45000014f, 0.24999994f, -0.24999978f),
    NiPoint3(-0.40000018f, 0.24999999f, -0.24999982f),
    NiPoint3(-0.35000020f, 0.24999996f, -0.24999979f),
    NiPoint3(-0.30000022f, 0.24999999f, -0.24999982f),
    NiPoint3(-0.25000018f, 0.24999997f, -0.24999979f),
    NiPoint3(-0.20000018f, 0.24999994f, -0.24999979f),
    NiPoint3(-0.15000018f, 0.24999996f, -0.24999979f),
    NiPoint3(-0.10000020f, 0.24999994f, -0.24999979f),
    NiPoint3(-0.050000206f, 0.24999994f, -0.24999979f),
    NiPoint3(-2.0066963e-007f, 0.24999996f, -0.24999979f),
    NiPoint3(-2.0066962e-007f, 0.24999994f, -0.19999978f),
    NiPoint3(-2.0066965e-007f, 0.24999999f, -0.14999981f),
    NiPoint3(-2.0066962e-007f, 0.24999996f, -0.099999800f),
    NiPoint3(-2.0066965e-007f, 0.24999999f, -0.049999807f),
    NiPoint3(-2.0066963e-007f, 0.24999997f, 2.0116568e-007f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.050000217f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.10000020f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.15000021f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.20000018f),
    NiPoint3(-2.0066963e-007f, 0.24999994f, 0.25000021f)
};

static unsigned int g_auiSpotLightIndices[] =
{
    0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15, 
    16, 17, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 
    26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 
    35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 
    44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 
    53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 59, 60, 60, 61, 61, 62, 62, 63, 
    63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 
    72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 
    81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 
    90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99 
};

//---------------------------------------------------------------------------
void CreateBoxVerts(NiPoint3* pkPoints, const NiTransform& kTrans,
    float fX, float fY, float fZ)
{
    float fHalfX = fX / 2.0f * kTrans.m_fScale;
    float fHalfY = fY / 2.0f * kTrans.m_fScale;
    float fHalfZ = fZ / 2.0f * kTrans.m_fScale;

    pkPoints[0] = kTrans.m_Rotate * NiPoint3(-fHalfX, -fHalfY, -fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[1] = kTrans.m_Rotate * NiPoint3(-fHalfX, fHalfY, -fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[2] = kTrans.m_Rotate * NiPoint3(fHalfX, fHalfY, -fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[3] = kTrans.m_Rotate * NiPoint3(fHalfX, -fHalfY, -fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[4] = kTrans.m_Rotate * NiPoint3(-fHalfX, -fHalfY, fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[5] = kTrans.m_Rotate * NiPoint3(-fHalfX, fHalfY, fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[6] = kTrans.m_Rotate * NiPoint3(fHalfX, fHalfY, fHalfZ) + 
        kTrans.m_Translate;
    pkPoints[7] = kTrans.m_Rotate * NiPoint3(fHalfX, -fHalfY, fHalfZ) + 
        kTrans.m_Translate;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiImmediateModeMacro::NiImmediateModeMacro(NiImmediateModeAdapter& kAdapter)
{
    m_pkAdapter = &kAdapter;
}
//---------------------------------------------------------------------------
NiImmediateModeMacro::~NiImmediateModeMacro()
{
    if (m_pkAdapter->IsDrawing())
        m_pkAdapter->EndDrawing();
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::SolidBox(const NiTransform& kLocalTrans, 
    float fX, float fY, float fZ)
{
    const unsigned int uiNumVerts = 8;
    const unsigned int uiNumIndices = 36;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiPoint3 akVerts[uiNumVerts];
    CreateBoxVerts(akVerts, kLocalTrans, fX, fY, fZ);

    unsigned int auiIndices[uiNumIndices] =
    {
        0, 2, 1,
        0, 3, 2,
        3, 6, 2,
        3, 7, 6,
        7, 5, 6,
        7, 4, 5,
        4, 1, 5,
        4, 0, 1,
        1, 6, 5,
        1, 2, 6,
        4, 3, 0,
        4, 7, 3
    };

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireBox(const NiTransform& kLocalTrans, 
    float fX, float fY ,float fZ)
{
    const unsigned int uiNumVerts = 8;
    const unsigned int uiNumIndices = 24;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3 akVerts[uiNumVerts];
    CreateBoxVerts(akVerts, kLocalTrans, fX, fY, fZ);

    unsigned int auiIndices[uiNumIndices] =
    {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        7, 4,
        4, 5,
        5, 6,
        6, 7,
        2, 6,
        3, 7,
        0, 4,
        1, 5,
    };

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireCamera(NiCamera* pkCamera, float fScaleMult)
{
    NIASSERT(pkCamera);

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    const unsigned int uiNumVerts = sizeof(g_akWireCameraVerts) / 
        sizeof(NiPoint3);
    const unsigned int uiNumIndices = sizeof(g_auiWireCameraIndices) / 
        sizeof(unsigned int);

    NiPoint3 akVerts[uiNumVerts];

    // Transform by camera position.
    NiTransform kTrans = pkCamera->GetWorldTransform();

    // Transform verts
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        akVerts[i] = kTrans.m_Rotate * g_akWireCameraVerts[i] * fScaleMult + 
            kTrans.m_Translate;
    }

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, 
        g_auiWireCameraIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireFrustum(NiCamera* pkCamera)
{
    NIASSERT(pkCamera);

    const NiFrustum& kFrust = pkCamera->GetViewFrustum();
    const NiTransform& kTrans = pkCamera->GetWorldTransform();

    WireFrustum(kFrust, kTrans);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireFrustum(const NiFrustum& kFrust,
    const NiTransform& kTrans)
{
    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    const unsigned int uiNumVerts = 8;
    const unsigned int uiNumIndices = 24;

    NiPoint3 kCameraDir;
    kTrans.m_Rotate.GetCol(0, kCameraDir);

    NiPoint3 akVerts[uiNumVerts];

    if (kFrust.m_bOrtho)
    {
        // World-space viewport translation vectors.
        NiPoint3 kViewTL = kTrans.m_Rotate * NiPoint3(0.0f, kFrust.m_fTop, 
            kFrust.m_fLeft) * kTrans.m_fScale;
        NiPoint3 kViewTR = kTrans.m_Rotate * NiPoint3(0.0f, kFrust.m_fTop, 
            kFrust.m_fRight) * kTrans.m_fScale;
        NiPoint3 kViewBL = kTrans.m_Rotate * NiPoint3(0.0f, kFrust.m_fBottom, 
            kFrust.m_fLeft) * kTrans.m_fScale;
        NiPoint3 kViewBR = kTrans.m_Rotate * NiPoint3(0.0f, kFrust.m_fBottom, 
            kFrust.m_fRight) * kTrans.m_fScale;

        akVerts[0] = kTrans.m_Translate + kViewBL + 
            kCameraDir * kFrust.m_fNear * kTrans.m_fScale;
        akVerts[1] = kTrans.m_Translate + kViewTL + 
            kCameraDir * kFrust.m_fNear * kTrans.m_fScale;
        akVerts[2] = kTrans.m_Translate + kViewTR + 
            kCameraDir * kFrust.m_fNear * kTrans.m_fScale;
        akVerts[3] = kTrans.m_Translate + kViewBR + 
            kCameraDir * kFrust.m_fNear * kTrans.m_fScale;

        akVerts[4] = kTrans.m_Translate + kViewBL + 
            kCameraDir * kFrust.m_fFar * kTrans.m_fScale;
        akVerts[5] = kTrans.m_Translate + kViewTL + 
            kCameraDir * kFrust.m_fFar * kTrans.m_fScale;
        akVerts[6] = kTrans.m_Translate + kViewTR + 
            kCameraDir * kFrust.m_fFar * kTrans.m_fScale;
        akVerts[7] = kTrans.m_Translate + kViewBR + 
            kCameraDir * kFrust.m_fFar * kTrans.m_fScale;

    }
    else
    {
        // World-space viewport direction vectors.
        NiPoint3 kViewTL = kTrans.m_Rotate * NiPoint3(1.0f, kFrust.m_fTop, 
            kFrust.m_fLeft) * kTrans.m_fScale;
        NiPoint3 kViewTR = kTrans.m_Rotate * NiPoint3(1.0f, kFrust.m_fTop, 
            kFrust.m_fRight) * kTrans.m_fScale;
        NiPoint3 kViewBL = kTrans.m_Rotate * NiPoint3(1.0f, kFrust.m_fBottom, 
            kFrust.m_fLeft) * kTrans.m_fScale;
        NiPoint3 kViewBR = kTrans.m_Rotate * NiPoint3(1.0f, kFrust.m_fBottom, 
            kFrust.m_fRight) * kTrans.m_fScale;

        // Viewport direction vectors unit length in camera direction, so
        // can be multiplied directly by distance to view planes.
        akVerts[0] = kTrans.m_Translate + kViewBL * kFrust.m_fNear;
        akVerts[1] = kTrans.m_Translate + kViewTL * kFrust.m_fNear;
        akVerts[2] = kTrans.m_Translate + kViewTR * kFrust.m_fNear;
        akVerts[3] = kTrans.m_Translate + kViewBR * kFrust.m_fNear;

        akVerts[4] = kTrans.m_Translate + kViewBL * kFrust.m_fFar;
        akVerts[5] = kTrans.m_Translate + kViewTL * kFrust.m_fFar;
        akVerts[6] = kTrans.m_Translate + kViewTR * kFrust.m_fFar;
        akVerts[7] = kTrans.m_Translate + kViewBR * kFrust.m_fFar;
    }

    unsigned int auiIndices[uiNumIndices] =
    {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        7, 4,
        4, 5,
        5, 6,
        6, 7,
        2, 6,
        3, 7,
        0, 4,
        1, 5,
    };

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WirePointLight(NiPointLight* pkLight, 
    float fScaleMult)
{
    NIASSERT(pkLight);

    const unsigned int uiNumVerts = sizeof(g_akWirePointLightVerts) / 
        sizeof(NiPoint3);
    const unsigned int uiNumIndices = sizeof(g_auiPointLightIndices) / 
        sizeof(unsigned int);

    NiPoint3 akVerts[uiNumVerts];

    const NiCamera* pkAdapterCam = m_pkAdapter->GetCurrentCamera();

    NiMatrix3 kRot = pkAdapterCam->GetWorldTransform().m_Rotate;

    // Transform by light position and billboard towards the camera.
    NiPoint3 kTranslate = pkLight->GetWorldTranslate();

    // Transform verts
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        akVerts[i] = kRot * g_akWirePointLightVerts[i] * fScaleMult + 
            kTranslate;
    }

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, 
        g_auiPointLightIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireSpotLight(NiSpotLight* pkLight, 
    float fScaleMult)
{
    NIASSERT(pkLight);

    const unsigned int uiNumVerts = sizeof(g_akWireSpotLightVerts) / 
        sizeof(NiPoint3);
    const unsigned int uiNumIndices = sizeof(g_auiSpotLightIndices) / 
        sizeof(unsigned int);

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3 akVerts[uiNumVerts];

    // Transform by camera position.
    NiTransform kTrans = pkLight->GetWorldTransform();

    // Transform verts
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        akVerts[i] = kTrans.m_Rotate * g_akWireSpotLightVerts[i] * 
            fScaleMult + kTrans.m_Translate;
    }

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, 
        g_auiSpotLightIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireDirectionalLight(NiDirectionalLight* pkLight,
    float fScaleMult)
{
    NIASSERT(pkLight);

    const unsigned int uiNumVerts = sizeof(g_akWireDirectionalLightVerts) / 
        sizeof(NiPoint3);
    const unsigned int uiNumIndices = sizeof(g_auiDirectionalLightIndices) / 
        sizeof(unsigned int);

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3 akVerts[uiNumVerts];

    // Transform by camera position.
    NiTransform kTrans = pkLight->GetWorldTransform();

    // Transform verts
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        akVerts[i] = kTrans.m_Rotate * g_akWireDirectionalLightVerts[i] * 
            fScaleMult + kTrans.m_Translate;
    }

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, 
        g_auiDirectionalLightIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::RotationalFrame(NiTransform& kTrans, 
    float fScaleMult)
{
    const unsigned int uiNumVerts = 12;
    const unsigned int uiNumIndices = 18;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3 akVerts[uiNumVerts];
    NiColorA akColors[uiNumVerts] =
    {
        NiColorA(1.0f, 0.0f, 0.0f, 1.0f),
        NiColorA(1.0f, 0.0f, 0.0f, 1.0f),
        NiColorA(1.0f, 0.0f, 0.0f, 1.0f),
        NiColorA(1.0f, 0.0f, 0.0f, 1.0f),

        NiColorA(0.0f, 1.0f, 0.0f, 1.0f),
        NiColorA(0.0f, 1.0f, 0.0f, 1.0f),
        NiColorA(0.0f, 1.0f, 0.0f, 1.0f),
        NiColorA(0.0f, 1.0f, 0.0f, 1.0f),

        NiColorA(0.0f, 0.0f, 1.0f, 1.0f),
        NiColorA(0.0f, 0.0f, 1.0f, 1.0f),
        NiColorA(0.0f, 0.0f, 1.0f, 1.0f),
        NiColorA(0.0f, 0.0f, 1.0f, 1.0f)
    };

    unsigned int auiIndices[uiNumIndices] =
    {
        0, 1,
        1, 2,
        1, 3,

        4, 5,
        5, 6,
        5, 7,

        8, 9,
        9, 10,
        9, 11
    };

    const NiCamera* pkCamera = m_pkAdapter->GetCurrentCamera();
    NiPoint3 kCamDir = pkCamera->GetWorldDirection();

    // Percent of length orthogonal to axis
    float fArrowWidth = 0.25f;
    // Percent of length parallel to axis
    float fArrowLength = 0.2f;

    // X-axis

    // World-space unit vector parallel to x-axis
    NiPoint3 kWorldX;
    kTrans.m_Rotate.GetCol(0, &kWorldX.x);
    kWorldX *= fScaleMult;
    // World-space unit vector orthogonal to both camera direction and x-axis
    // This is equivalent to NiBillboardNode::ROTATE_ABOUT_UP which will try
    // to make the normal of the plane of the arrow head parallel to the
    // camera direction, while only letting it rotate around the x-axis.
    NiPoint3 kCrossX = fArrowWidth * kCamDir.UnitCross(kWorldX);
    // World-space position of the ends of the arrow tips, not including
    // any components orthogonal to the x-axis.
    NiPoint3 kArrowX = (1.0f - fArrowLength) * kWorldX + kTrans.m_Translate;
    akVerts[0] = kTrans.m_Translate;
    akVerts[1] = kTrans.m_Translate + kWorldX;
    akVerts[2] = kArrowX + kCrossX;
    akVerts[3] = kArrowX - kCrossX;

    // Y-axis
    NiPoint3 kWorldY;
    kTrans.m_Rotate.GetCol(1, &kWorldY.x);
    kWorldY *= fScaleMult;
    NiPoint3 kCrossY = fArrowWidth * kCamDir.UnitCross(kWorldY);
    NiPoint3 kArrowY = (1.0f - fArrowLength) * kWorldY + kTrans.m_Translate;
    akVerts[4] = kTrans.m_Translate;
    akVerts[5] = kTrans.m_Translate + kWorldY;
    akVerts[6] = kArrowY + kCrossY;
    akVerts[7] = kArrowY - kCrossY;

    // Z-axis
    NiPoint3 kWorldZ;
    kTrans.m_Rotate.GetCol(2, &kWorldZ.x);
    kWorldZ *= fScaleMult;
    NiPoint3 kCrossZ = fArrowWidth * kCamDir.UnitCross(kWorldZ);
    NiPoint3 kArrowZ = (1.0f - fArrowLength) * kWorldZ + kTrans.m_Translate;
    akVerts[8] = kTrans.m_Translate;
    akVerts[9] = kTrans.m_Translate + kWorldZ;
    akVerts[10] = kArrowZ + kCrossZ;
    akVerts[11] = kArrowZ - kCrossZ;

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices,
        akColors);
}
//---------------------------------------------------------------------------
NiUInt32 GetLevel(NiAVObject* pkStopAt, NiAVObject* pkCurrent, 
    NiUInt32 uiCurrentLevel)
{
    if (pkStopAt == pkCurrent)
        return uiCurrentLevel;
    if (pkCurrent->GetParent() != NULL)
        return GetLevel(pkStopAt, (NiAVObject*)pkCurrent->GetParent(), 
        uiCurrentLevel + 1);
    return uiCurrentLevel;
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireBoneHierarchy(NiMesh* pkMesh)
{
    NiSkinningMeshModifier* pkModifier = NiGetModifier(NiSkinningMeshModifier,
        pkMesh);

    if (!pkModifier)
        return;

    NiAVObject* pkRoot = pkModifier->GetRootBoneParent();
    NiAVObject** ppkBones = pkModifier->GetBones();
    NiUInt32 uiBoneCount = pkModifier->GetBoneCount();

    const NiUInt32 uiNumColors = 8;
    NiColorA akColorsByLevel [uiNumColors] = 
    {
        NiColorA(161.0f / 255.0f, 106.0f / 255.0f,  48.0f / 255.0f, 1.0f),
        NiColorA(158.0f / 255.0f, 161.0f / 255.0f,  48.0f / 255.0f, 1.0f),
        NiColorA(104.0f / 255.0f, 161.0f / 255.0f,  48.0f / 255.0f, 1.0f),
        NiColorA( 48.0f / 255.0f, 161.0f / 255.0f,  93.0f / 255.0f, 1.0f),
        NiColorA( 48.0f / 255.0f, 161.0f / 255.0f, 161.0f / 255.0f, 1.0f),
        NiColorA( 48.0f / 255.0f, 103.0f / 255.0f, 161.0f / 255.0f, 1.0f),
        NiColorA(111.0f / 255.0f,  48.0f / 255.0f, 161.0f / 255.0f, 1.0f),
        NiColorA(161.0f / 255.0f,  48.0f / 255.0f, 106.0f / 255.0f, 1.0f)
    };

    for (NiUInt32 ui = 0; ui < uiBoneCount; ui++)
    {
        NiAVObject* pkObj = ppkBones[ui];

        if (pkObj != NULL)
        {
            NiUInt32 uiLevel = GetLevel(pkRoot, pkObj, 0);
            NiColorA kColor = akColorsByLevel[uiLevel % uiNumColors];

            NiAVObject* pkParent = (NiAVObject*)pkObj->GetParent();
            if (pkParent)
            {
                CreateBone(pkParent->GetWorldTransform(), 
                    pkObj->GetWorldTransform(), kColor);
            }
            else
            {
                NiTransform kTransform;
                kTransform.MakeIdentity();
                CreateBone(kTransform, pkObj->GetWorldTransform(), 
                    kColor);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::CreateBone(const NiTransform& kParent, 
    const NiTransform& kChild, const NiColorA& kColor)
{
    const unsigned int uiNumVerts = 2;
    const unsigned int uiNumIndices = 2;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3 akVerts[uiNumVerts];
    NiColorA akColors[uiNumVerts];

    for (NiUInt32 ui = 0; ui < uiNumVerts; ui++)
    {
        akColors[ui] = kColor;
    };

    unsigned int auiIndices[uiNumIndices] =
    {
        0, 1,
    };

    akVerts[0] = kParent.m_Translate;
    akVerts[1] = kChild.m_Translate;

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices,
        akColors);
}
//---------------------------------------------------------------------------
float NiImmediateModeMacro::GetScreenScaleFactor(const NiAVObject* pkObj,
    const NiImmediateModeAdapter* pkAdapter)
{
    NIASSERT(pkAdapter != NULL);
    const NiCamera* pkAdapterCam = pkAdapter->GetCurrentCamera();
    NIASSERT(pkAdapterCam);
    const NiFrustum& kFrust = pkAdapterCam->GetViewFrustum();

    if (kFrust.m_bOrtho)
    {
        return pkAdapterCam->GetViewFrustum().m_fRight * 2.0f;
    }
    else
    {
        float fCamDistance = (pkAdapterCam->GetWorldLocation() -
            pkObj->GetWorldTransform().m_Translate).Length();

        return fCamDistance *
            pkAdapterCam->GetViewFrustum().m_fRight * 2.0f;
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::CreateSphereVerts(
    NiPoint3* pkPoints, 
    const NiPoint3& kCenter, 
    float fRadius, 
    unsigned int uiNumSlices, 
    unsigned int uiNumStacks)
{
    // Stacks along Z-axis.
    // Slices around Z-axis.

    *pkPoints++ = kCenter + NiPoint3(0, 0, fRadius);

    for (unsigned int uiSt = 0; uiSt < uiNumStacks - 1; uiSt++)
    {
        float fAngle = (float)(uiSt + 1) / (float)(uiNumStacks) * NI_PI;

        float fZ;
        float fStackRadius;
        NiSinCos(fAngle, fStackRadius, fZ);
        fZ *= fRadius;
        fStackRadius *= fRadius;

        for (unsigned int uiSl = 0; uiSl < uiNumSlices; uiSl++)
        {
            float fAng = NI_TWO_PI * (float)uiSl / (float)uiNumSlices;
            float fX;
            float fY;
            NiSinCos(fAng, fX, fY);
            
            pkPoints[uiSl] = NiPoint3(fX * fStackRadius, fY * fStackRadius,
                fZ) + kCenter;
        }

        pkPoints += uiNumSlices;
    }

    *pkPoints++ = kCenter + NiPoint3(0, 0, -fRadius);
}
//---------------------------------------------------------------------------
inline void NiImmediateModeMacro::CreateConeVerts(
    NiPoint3* pkPoints, 
    const NiTransform& kLocalTrans, 
    float fLength, 
    float fRadius, 
    unsigned int uiNumSlices)
{
    float fScaledLength = kLocalTrans.m_fScale * fLength;
    float fScaledRadius = kLocalTrans.m_fScale * fRadius;

    for (unsigned int i = 0; i < uiNumSlices; i++)
    {
        float fY;
        float fZ;

        NiSinCos((float)i * NI_TWO_PI / (float)uiNumSlices, fY, fZ);

        pkPoints[i] = kLocalTrans.m_Translate + 
            kLocalTrans.m_Rotate * 
            NiPoint3(fScaledLength, fScaledRadius * fY, fScaledRadius * fZ);
    }

    pkPoints[uiNumSlices] = kLocalTrans.m_Translate + 
        kLocalTrans.m_Rotate * NiPoint3(fScaledLength, 0.0f, 0.0f);
    pkPoints[uiNumSlices] = kLocalTrans.m_Translate;
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireSphere(
    const NiPoint3& kCenter, 
    float fRadius, 
    unsigned int uiNumSlices, 
    unsigned int uiNumStacks)
{
    unsigned int uiNumVerts = 2 + (uiNumSlices) * (uiNumStacks-1);
    unsigned int uiNumIndices = 2 + (uiNumStacks-1) * (uiNumSlices+1) + 
        (uiNumSlices-1)*(uiNumStacks);

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINESTRIPS);

    NiPoint3* akVerts = NiStackAlloc(NiPoint3, uiNumVerts);
    CreateSphereVerts(akVerts, kCenter, fRadius, uiNumSlices, uiNumStacks);

    unsigned int* auiIndices = NiStackAlloc(unsigned int, uiNumIndices);

    // pointer to walk through indices
    unsigned int* puiIndices = auiIndices;

    // Go around stacks first, while going up slice 0
    {
        // start at the bottom
        *puiIndices++ = 0;

        for (unsigned int uiSt = 0; uiSt < uiNumStacks - 1; uiSt++)
        {
            unsigned int uiStackOffset = 1 + uiSt * uiNumSlices;
            for (unsigned int uiSl = 0; uiSl < uiNumSlices; uiSl++)
            {
                puiIndices[uiSl] = uiSl + uiStackOffset;
            }
            puiIndices[uiNumSlices] = uiStackOffset;
            puiIndices += uiNumSlices + 1;
        }

        // top it off
        *puiIndices++ = uiNumVerts - 1;
    }

    // Go down slice 1, up slice 2, down slice 3, etc...
    for (unsigned int uiSl = 1; uiSl < uiNumSlices; uiSl++)
    {
        if (uiSl % 2 == 1)
        {
            // go down slice
            for (int iSt = uiNumStacks - 2; iSt >= 0; iSt--)
            {
                *puiIndices++ = uiSl + 1 + iSt * uiNumSlices;
            }

            // bottom
            *puiIndices++ = 0;
        }
        else
        {
            // go up slice
            for (unsigned int uiSt = 0; uiSt < uiNumStacks - 1; uiSt++)
            {
                *puiIndices++ = uiSl + 1 + uiSt * uiNumSlices;
            }

            // top
            *puiIndices++ = uiNumVerts - 1;
        }
    }

    NIASSERT(puiIndices == &auiIndices[uiNumIndices]);

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);

    NiStackFree(akVerts);
    NiStackFree(auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::SolidSphere(
    const NiPoint3& kCenter, 
    float fRadius, 
    unsigned int uiNumSlices, 
    unsigned int uiNumStacks)
{
    unsigned int uiNumVerts = 2 + (uiNumSlices) * (uiNumStacks-1);
    // Top and bottom stacks are triangles, intermediates are quads.
    unsigned int uiNumIndices = (uiNumStacks-1) * uiNumSlices * 6;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiPoint3* akVerts = NiStackAlloc(NiPoint3, uiNumVerts);
    CreateSphereVerts(akVerts, kCenter, fRadius, uiNumSlices, uiNumStacks);

    unsigned int* auiIndices = NiStackAlloc(unsigned int, uiNumIndices);
    unsigned int* puiIndices = auiIndices;

    for (unsigned int i = 0; i < uiNumSlices; i++)
    {
        unsigned int uiOffset1 = i % uiNumSlices;
        unsigned int uiOffset2 = (i+1) % uiNumSlices;

        const unsigned int uiBottomOffset = 1;
        unsigned int uiTopOffset = uiNumVerts - 1 - uiNumSlices;

        // Bottom
        puiIndices[0] = 0;
        puiIndices[1] = uiBottomOffset + uiOffset2;
        puiIndices[2] = uiBottomOffset + uiOffset1;

        // Top
        puiIndices[3] = uiNumVerts - 1;
        puiIndices[4] = uiTopOffset + uiOffset1;
        puiIndices[5] = uiTopOffset + uiOffset2;

        puiIndices += 6;
    }

    // Intermediate stacks
    for (unsigned int uiSl = 0; uiSl < uiNumSlices; uiSl++)
    {
        unsigned int uiSlice1 = uiSl;
        unsigned int uiSlice2 = (uiSl + 1) % uiNumSlices;

        for (unsigned int uiSt = 0; uiSt < uiNumStacks - 2; uiSt++)
        {
            unsigned int uiLowerStackOffset = 1 + uiSt * uiNumSlices;
            unsigned int uiUpperStackOffset = uiLowerStackOffset + uiNumSlices;

            puiIndices[0] = uiLowerStackOffset + uiSlice1;
            puiIndices[1] = uiUpperStackOffset + uiSlice2;
            puiIndices[2] = uiUpperStackOffset + uiSlice1;

            puiIndices[3] = uiLowerStackOffset + uiSlice1;
            puiIndices[4] = uiLowerStackOffset + uiSlice2;
            puiIndices[5] = uiUpperStackOffset + uiSlice2;

            puiIndices += 6;
        }
    }

    NIASSERT(&auiIndices[uiNumIndices] == puiIndices);

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);

    NiStackFree(akVerts);
    NiStackFree(auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::SolidCone(
    const NiTransform& kLocalTrans, 
    float fLength, 
    float fRadius, 
    unsigned int uiNumSlices)
{
    unsigned int uiNumVerts = uiNumSlices + 2;
    unsigned int uiNumIndices = uiNumSlices * 6;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiPoint3* akVerts = NiStackAlloc(NiPoint3, uiNumVerts);
    CreateConeVerts(akVerts, kLocalTrans, fLength, fRadius, uiNumSlices);

    unsigned int* auiIndices = NiStackAlloc(unsigned int, uiNumIndices);
    unsigned int* puiIndices = auiIndices;

    for (unsigned int i = 0; i < uiNumSlices; i++)
    {
        unsigned int uiVertPlusOne = (i+1) % uiNumSlices;

        // cone
        puiIndices[0] = i;
        puiIndices[1] = uiVertPlusOne;
        puiIndices[2] = uiNumSlices;

        // cone base
        puiIndices[3] = uiNumSlices - 1;
        puiIndices[4] = uiVertPlusOne;
        puiIndices[5] = i;

        puiIndices += 6;
    }

    NIASSERT(puiIndices == &auiIndices[uiNumIndices]);

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);

    NiStackFree(akVerts);
    NiStackFree(auiIndices);
}
//---------------------------------------------------------------------------
void NiImmediateModeMacro::WireCone(
    const NiTransform& kLocalTrans, 
    float fLength, 
    float fRadius, 
    unsigned int uiNumSlices)
{
    unsigned int uiNumVerts = uiNumSlices + 2;
    unsigned int uiNumIndices = uiNumSlices * 4;

    ForceAdapterIntoMode(NiPrimitiveType::PRIMITIVE_LINES);

    NiPoint3* akVerts = NiStackAlloc(NiPoint3, uiNumVerts);
    CreateConeVerts(akVerts, kLocalTrans, fLength, fRadius, uiNumSlices);

    unsigned int* auiIndices = NiStackAlloc(unsigned int, uiNumIndices);
    unsigned int* puiIndices = auiIndices;
    
    for (unsigned int i = 0; i < uiNumSlices; i++)
    {
        // Line around cone base
        puiIndices[0] = i;
        puiIndices[1] = (i+1) % uiNumSlices;

        // Line to cone top
        puiIndices[2] = i;
        puiIndices[3] = uiNumSlices;

        puiIndices += 4;
    }

    NIASSERT(puiIndices == &auiIndices[uiNumIndices]);

    m_pkAdapter->Append(uiNumVerts, akVerts, uiNumIndices, auiIndices);

    NiStackFree(akVerts);
    NiStackFree(auiIndices);
}
//---------------------------------------------------------------------------
