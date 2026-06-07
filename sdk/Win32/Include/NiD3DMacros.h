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
//---------------------------------------------------------------------------
#ifndef NID3DMACROS_H
#define NID3DMACROS_H

#include "NiD3DDefines.h"

//  Common D3D-related functions wrapped in macros.
//  Primarily interface functionality, such as reference/releasing, etc.

//  Define this to log out interface reference counting...
//#define DEBUG_INTERFACE_POINTERS

#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))
//  These are to do reference count checking during debug development
//  char buffer defined in Ni***Renderer.cpp
extern char g_szIFDebugBuff[1024];
extern int g_iDeviceRefCount;
extern D3DDevicePtr g_pkD3DDevice;

#define D3D_SAFE_RELEASE(pInterface,pFile,iLine)                            \
    {                                                                       \
        if (pInterface)                                                     \
        {                                                                   \
            if ((unsigned int)pInterface == (unsigned int)g_pkD3DDevice)    \
                g_iDeviceRefCount--;                                        \
            UINT uiRefCnt = pInterface->Release();                          \
            if (uiRefCnt != 0)                                              \
            {                                                               \
                if ((unsigned int)pInterface ==                             \
                    (unsigned int)g_pkD3DDevice)                            \
                {                                                           \
                    NiSprintf(g_szIFDebugBuff, 1024,                        \
                        "DEVICE Release - 0x%08x - "                        \
                        "NiRefCnt = %3d - RefCnt = %3d @ %4d - %s\n",       \
                        pInterface, g_iDeviceRefCount, uiRefCnt, iLine,     \
                        pFile);                                             \
                }                                                           \
                else                                                        \
                {                                                           \
                    NiSprintf(g_szIFDebugBuff, 1024,                        \
                        "Release - 0x%08x - RefCnt = "                      \
                        "%3d @ %4d - %s\n", pInterface, uiRefCnt, iLine,    \
                        pFile);                                             \
                }                                                           \
                NiOutputDebugString(g_szIFDebugBuff);                       \
            }                                                               \
        }                                                                   \
    }

#define D3D_POINTER_RELEASE(pInterface)                                     \
    D3D_SAFE_RELEASE(pInterface,__FILE__,__LINE__)

#define D3D_SAFE_REFERENCE(pInterface,pFile,iLine)                          \
    {                                                                       \
        if (pInterface)                                                     \
        {                                                                   \
            if ((unsigned int)pInterface == (unsigned int)g_pkD3DDevice)    \
                g_iDeviceRefCount++;                                        \
            UINT uiRefCnt = pInterface->AddRef();                           \
            if ((unsigned int)pInterface == (unsigned int)g_pkD3DDevice)    \
            {                                                               \
                NiSprintf(g_szIFDebugBuff, 1024,                            \
                    "DEVICE AddRef - 0x%08x - "                             \
                    "NiRefCnt = %3d - RefCnt = %3d @ %4d - %s\n",           \
                    pInterface, g_iDeviceRefCount, uiRefCnt, iLine,         \
                    pFile);                                                 \
            }                                                               \
            else                                                            \
            {                                                               \
                NiSprintf(g_szIFDebugBuff, 1024,                            \
                    "AddRef - 0x%08x - RefCnt != 0 "                        \
                    "- = %3d @ %4d - %s\n", pInterface, uiRefCnt, iLine,    \
                    pFile);                                                 \
            }                                                               \
            NiOutputDebugString(g_szIFDebugBuff);                           \
        }                                                                   \
    }

#define D3D_POINTER_REFERENCE(pInterface)                                   \
    D3D_SAFE_REFERENCE(pInterface,__FILE__,__LINE__)

#define D3D_SAFE_CHECK(pInterface,pFile,iLine)                              \
    {                                                                       \
        pInterface->AddRef();                                               \
        UINT uiRefCnt = pInterface->Release();                              \
        if ((unsigned int)pInterface == (unsigned int)g_pkD3DDevice)        \
        {                                                                   \
            NiSprintf(g_szIFDebugBuff, 1024, "DEVICE RefCheck - 0x%08x - "  \
                "NiRefCnt = %3d - RefCnt = %3d @ %4d - %s\n",               \
                pInterface, g_iDeviceRefCount, uiRefCnt, iLine,             \
                pFile);                                                     \
        }                                                                   \
        else                                                                \
        {                                                                   \
            NiSprintf(g_szIFDebugBuff, 1024,                                \
                "RefCheck - 0x%08x - RefCnt = "                             \
                "%3d @ %4d - %s\n", pInterface, uiRefCnt, iLine, pFile);    \
        }                                                                   \
        NiOutputDebugString(g_szIFDebugBuff);                               \
    }

#define D3D_POINTER_CHECK(pInterface)                                       \
    D3D_SAFE_CHECK(pInterface,__FILE__,__LINE__)

#else   //#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))

#define D3D_POINTER_RELEASE(pInterface)        pInterface->Release()
#define D3D_POINTER_REFERENCE(pInterface)      pInterface->AddRef()
#define D3D_POINTER_CHECK(pInterface)          {}

#endif  //#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))

#endif  //#ifndef NID3DMACROS_H