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

#ifndef NID3DUTILITY_H
#define NID3DUTILITY_H

#include "NiD3DDefines.h"
#include "NiUniversalTypes.h"

#include <NiMatrix3.h>
#include <NiTransform.h>

class NiBinaryStream;
class NiRenderObject;
class NiTriShape;
class NiTriStrips;
class NiPoint3;
class NiPoint2;

class NID3D_ENTRY NiD3DUtility : public NiMemObject
{
public:
    NiD3DUtility();
    ~NiD3DUtility();

    // Log
    static void Log(char* pcFmt, ...);
    static bool GetLogEnabled();
    static void SetLogEnabled(bool bEnabled);

    // GetFileXXX
    static bool GetSplitPath(const char* pcFilename, char* pcDrive, 
        char* pcDir, char* pcFname, char* pcExt);
    static bool GetFilePath(const char* pcFilename, char* pcPath,
        unsigned int uiMaxLen);
    static bool GetFileName(const char* pcFilename, char* pcName,
        unsigned int uiMaxLen);
    static bool GetFileExtension(const char* pcFilename, char* pcExt,
        unsigned int uiMaxLen);

    // String setting helper function
    static unsigned int SetString(char*& pcDest, unsigned int uiDestSize, 
        const char* pcSrc);

    // Write to stream
    static bool WriteData(NiBinaryStream& kStream, void* pvData,
        unsigned int uiSize, unsigned int uiComponentSize = 1);

    // Read from stream
    static bool ReadData(NiBinaryStream& kStream, void*& pvData,
        unsigned int& uiSize);
    static bool AllocateAndReadData(NiBinaryStream& kStream, void*& pvData,
        unsigned int& uiDataSize, unsigned int uiComponentSize = 1);

    // Unicode character conversion
    static bool ConvertCHARToWCHAR(WCHAR* pwDestination, 
        const char* pcSource, unsigned int uiDestChar);
    static bool ConvertWCHARToCHAR(char* pcDestination, 
        const WCHAR* pwSource, unsigned int uiDestChar);
    static bool ConvertTCHARToCHAR(char* pcDestination, 
        const TCHAR* ptSource, unsigned int uiDestChar);
    static bool ConvertTCHARToWCHAR(WCHAR* pwDestination, 
        const TCHAR* ptSource, unsigned int uiDestChar);
    static bool ConvertCHARToTCHAR(TCHAR* ptDestination, 
        const char* pcSource, unsigned int uiDestChar);
    static bool ConvertWCHARToTCHAR(TCHAR* ptDestination, 
        const WCHAR* pwSource, unsigned int uiDestChar);

    // Matrix conversion
    static void GetD3DFromNi(D3DMATRIX& kD3D, const NiTransform& kNi);
    static void GetD3DFromNi(D3DMATRIX& kD3D, const NiMatrix3& kNiRot,
        const NiPoint3& kNiTrans, float fNiScale);
    static void GetD3DTransposeFromNi(D3DMATRIX& kD3D,
        const NiTransform& kNi);
    static void GetD3DTransposeFromNi(D3DMATRIX& kD3D,
        const NiMatrix3& kNiRot, const NiPoint3& kNiTrans, float fNiScale);

    // Matrix operations
    static D3DXMATRIX* D3DXMatrixMultiply_NonAccelerated(
        D3DXMATRIX * NI_RESTRICT pkOut, 
        const D3DXMATRIX * NI_RESTRICT pkM1, 
        const D3DXMATRIX * NI_RESTRICT pkM2);

protected:
    static bool ms_bLogEnabled;
};

#include "NiD3DUtility.inl"

#endif  //NID3DUTILITY_H
