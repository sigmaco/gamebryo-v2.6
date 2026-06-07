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
inline void NiSleep(unsigned int uiMilliseconds)
{
    Sleep(uiMilliseconds);
}
//---------------------------------------------------------------------------
inline void NiYieldThread()
{
    // Preferably, we could call SwitchToThread here, but that requires 
    // knowing that your target platform is at least WinNT.
    Sleep(0);
}
//---------------------------------------------------------------------------
inline void NiPrefetch128(void*)
{
    // Intentionally blank at this time.
}
//---------------------------------------------------------------------------
inline NiInt32 NiAtomicIncrement(NiInt32& i32Value)
{
    return InterlockedIncrement((LONG*)&i32Value);
}
//---------------------------------------------------------------------------
inline NiInt32 NiAtomicDecrement(NiInt32& i32Value)
{
    return InterlockedDecrement((LONG*)&i32Value);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiAtomicIncrement(NiUInt32& ui32Value)
{
    return InterlockedIncrement((LONG*)&ui32Value);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiAtomicDecrement(NiUInt32& ui32Value)
{
    NIASSERT(ui32Value > 0);
    return InterlockedDecrement((LONG*)&ui32Value);
}
//---------------------------------------------------------------------------
inline float NiSelectGE(float fCondition, float fResultIfGreaterThanEqualZero, 
    float fResultIfLessThanZero)
{
    return fCondition >= 0 ? 
        fResultIfGreaterThanEqualZero : fResultIfLessThanZero;
}
//---------------------------------------------------------------------------
inline float NiSelectGE(int iCondition, float fResultIfGreaterThanEqualZero, 
    float fResultIfLessThanZero)
{
    return iCondition >= 0 ? 
        fResultIfGreaterThanEqualZero : fResultIfLessThanZero;
}
//---------------------------------------------------------------------------
inline int NiSelect(bool iCondition, int iResultIfTrue, int iResultIfFalse)
{
    return iCondition ? iResultIfTrue : iResultIfFalse;
}
//---------------------------------------------------------------------------
inline float NiSelect(bool iCondition, float fResultIfTrue, 
    float fResultIfFalse)
{
    return iCondition ? fResultIfTrue : fResultIfFalse;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiInt32ToUInt8(NiInt32 iNum)
{
    NIASSERT(iNum >= 0);
    NIASSERT(iNum <= UCHAR_MAX);
    return (NiUInt8)(iNum & 0xFF);
}
//---------------------------------------------------------------------------
inline NiUInt16 NiInt32ToUInt16(NiInt32 iNum)
{
    NIASSERT(iNum >= 0);
    NIASSERT(iNum <= USHRT_MAX);
    return (NiUInt16)(iNum & 0xFFFF);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiInt32ToUInt32(NiInt32 iNum)
{
    return (NiUInt32)iNum;
}
//---------------------------------------------------------------------------
inline NiInt8 NiInt32ToInt8(NiInt32 iNum)
{
    NIASSERT(iNum >= SCHAR_MIN);
    NIASSERT(iNum <= SCHAR_MAX);

    // Twos complement numbers fully extend the sign bit. If we pass the
    // assertions above, we are within range and a mask is sufficient.
    return (NiInt8)(iNum & 0xFF);
}
//---------------------------------------------------------------------------
inline NiInt16 NiInt32ToInt16(NiInt32 iNum)
{
    NIASSERT(iNum >= SHRT_MIN);
    NIASSERT(iNum <= SHRT_MAX);

    // Twos complement numbers fully extend the sign bit. If we pass the
    // assertions above, we are within range and a mask is sufficient.
    return (NiInt16)(iNum & 0xFFFF);
}
//---------------------------------------------------------------------------
inline unsigned long NiGetCurrentThreadId()
{
    return (unsigned long)GetCurrentThreadId();
}
//---------------------------------------------------------------------------
inline char* NiStrcpy(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcpy_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    NIASSERT(stDestSize != 0);

    size_t stSrcLen = strlen(pcSrc);
    NIASSERT(stDestSize > stSrcLen); // > because need null character

    size_t stWrite;
    
    if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
        stWrite = stDestSize;
    else 
        stWrite = stSrcLen + 1;

    char *pcResult = strncpy(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrncpy(char* pcDest, size_t stDestSize, const char* pcSrc, 
    size_t stCount)
{
#if _MSC_VER >= 1400
    strncpy_s(pcDest, stDestSize, pcSrc, stCount);
    return pcDest;
#else // #if _MSC_VER >= 1400
    
    NIASSERT(pcDest != 0 && stDestSize != 0);
    NIASSERT(stCount < stDestSize || stCount == NI_TRUNCATE);

    if (stCount >= stDestSize)
    {
        if (stCount != NI_TRUNCATE)
        {
            pcDest[0] = '\0';
            return pcDest;
        }
        else
        {
            stCount = stDestSize - 1;
        }
    }

    char* pcResult = strncpy(pcDest, pcSrc, stCount);
    pcResult[stCount] = '\0';

    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrcat(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcat_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    size_t stSrcLen = strlen(pcSrc);
    size_t stDestLen = strlen(pcDest);
    NIASSERT(stSrcLen + stDestLen <= stDestSize - 1);

    size_t stWrite = stDestSize - 1 - stDestLen;  

    char* pcResult = strncat(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;

#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrtok(char* pcString, const char* pcDelimit, 
    char** ppcContext)
{
#if _MSC_VER >= 1400
    return strtok_s(pcString, pcDelimit, ppcContext);
#else // #if _MSC_VER >= 1400
    return strtok(pcString, pcDelimit);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline int NiSprintf(char* pcDest, size_t stDestSize, const char* pcFormat, 
    ...)
{
    NIASSERT(pcFormat);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    int iRet = NiVsprintf(pcDest, stDestSize, pcFormat, kArgs);
    va_end(kArgs);

    return iRet;
}
//---------------------------------------------------------------------------
inline int NiVsprintf(char* pcDest, size_t stDestSize, const char* pcFormat,
    va_list kArgs)
{
    return NiVsnprintf(pcDest, stDestSize, NI_TRUNCATE, pcFormat, kArgs);
}
//---------------------------------------------------------------------------
inline int NiSnprintf(char* pcDest, size_t stDestSize, size_t stCount,
    const char* pcFormat, ...)
{
    NIASSERT(pcFormat);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    int iRet = NiVsnprintf(pcDest, stDestSize, stCount, pcFormat, kArgs);
    va_end(kArgs);

    return iRet;
}
//---------------------------------------------------------------------------
inline int NiVsnprintf(char* pcDest, size_t stDestSize, size_t stCount,
    const char* pcFormat, va_list kArgs)
{
    if (stDestSize == 0)
    {
        return 0;
    }

    NIASSERT(pcDest);
    NIASSERT(stCount < stDestSize || stCount == NI_TRUNCATE);
    NIASSERT(pcFormat);

    // Ensure that input buffer is cleared out.
    pcDest[0] = '\0';

    bool bTruncate = (stCount == NI_TRUNCATE);

#if _MSC_VER >= 1400
    int iRet = vsnprintf_s(pcDest, stDestSize, stCount, pcFormat, kArgs);
#else   // _MSC_VER >= 1400
    if (bTruncate)
    {
        stCount = stDestSize - 1;
    }
    int iRet = _vsnprintf(pcDest, stCount, pcFormat, kArgs);
#endif  // _MSC_VER >= 1400

    if (iRet == -1 && !bTruncate)
    {
        iRet = (int)stCount;
    }

#if _MSC_VER < 1400
    // Ensure that the string ends in a null character.
    if (iRet == -1)
    {
        pcDest[stDestSize - 1] = '\0';
    }
    else
    {
        pcDest[iRet] = '\0';
    }
#endif  // #if _MSC_VER < 1400

    return iRet;
}
//-------------------------------------------------------------------------
// Use 128 bit aligned arrays with size multiples of 4 floats on SSE
// capable PCs. Use on __declspec(align(16)) float or DWORD arrays

inline void NiMemcpyFloatArraySSE(float* pDst, const float* pSrc, int SizeInFloats)
{
    int iCount = SizeInFloats >> 2;          

    __asm {
        mov     ecx,    iCount       
        mov     edi,    pDst         
        mov     esi,    pSrc         

    loop1:
        movaps  xmm0,   [esi]       
        movaps  [edi],  xmm0        

        add     esi,    16
        add     edi,    16

        dec     ecx                 
        jnz     loop1
    }
}                                                                 
//-------------------------------------------------------------------------
// Use 32 bit aligned arrays with size multiples of 4 bytes.
// Use on __declspec(align(4)) float or DWORD arrays

inline void NiMemcpyFloatArray(float* pDst, const float* pSrc, int SizeInFloats)
{
    int iBytes = sizeof(float) * SizeInFloats;

    __asm {
        mov     ecx,    iBytes       
        shr     ecx,    2           

        cld                         // clear direction flag
        mov     esi,    pSrc         
        mov     edi,    pDst         
        rep     movsd               // copy dword at a time
    }
}

//-------------------------------------------------------------------------

inline int NiMemcpyBytes(long* pDst,const long* pSrc, unsigned long SizeInBytes)
{

    _asm
    {
        pusha;

        mov  ebx,SizeInBytes;
        mov  edi,pDst;
        mov  esi,pSrc;
                    
looping:
        // read to register
        mov ecx, [esi]; 
        // write from register
        mov [edi], ecx; 

        add esi, 1;
        add edi, 1;
        dec ebx;

        jnz  looping;

        popa;
    }
    return 0;
}      

//---------------------------------------------------------------------------
inline int NiMemcpy(void* pvDest, size_t stDestSize, const void* pvSrc, 
    size_t stCount)
{
    int iRet = 0;

#if _MSC_VER >= 1400
    iRet = memcpy_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
    if (stDestSize < stCount)
        iRet = -1;
    else
        memcpy(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

    NIASSERT(iRet != -1);
    return iRet;
}
//---------------------------------------------------------------------------
inline int NiMemcpy(void* pvDest, const void* pvSrc, size_t stCount)
{
    // This version of NiMemcpy is to be used to retrofit existing code,
    // however, the other version of NiMemcpy should be used whenever
    // possible.
    return NiMemcpy(pvDest, stCount, pvSrc, stCount);
}
//---------------------------------------------------------------------------
inline int NiMemmove(void* pvDest, size_t stDestSize, const void* pvSrc, 
    size_t stCount)
{
    int iRet = 0;

#if _MSC_VER >= 1400
    iRet = memmove_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
    if (stDestSize < stCount)
        iRet = -1;
    else
        memmove(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

    NIASSERT(iRet != -1);
    return iRet;
}
//---------------------------------------------------------------------------
// NiWChar version of function to copy one string to another.
inline NiWChar* NiWStrcpy(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc)
{

#if _MSC_VER >= 1400
    wcscpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
    return pkDest;
#else // #if _MSC_VER >= 1400
    NIASSERT(stDestSize != 0);

    size_t stSrcLen = wcslen(pkSrc);
    NIASSERT(stDestSize > stSrcLen); // > because need null character

    size_t stWrite;
  
    if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
        stWrite = stDestSize;
    else 
        stWrite = stSrcLen + 1;

    NiWChar* pkResult = wcsncpy(pkDest, pkSrc, stWrite);
    pkResult[stDestSize - 1] = '\0';
    return pkResult;
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to copy characters of one string to another.
inline NiWChar* NiWStrncpy(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc, size_t stCount)
{

#if _MSC_VER >= 1400
    wcsncpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc, stCount);
    return pkDest;
#else // #if _MSC_VER >= 1400
    NIASSERT(pkDest != 0 && stDestSize != 0);
    NIASSERT(stCount < stDestSize || stCount == NI_TRUNCATE);

    if (stCount >= stDestSize)
    {
        if (stCount != NI_TRUNCATE)
        {
            pkDest[0] = '\0';
            return pkDest;
        }
        else
        {
            stCount = stDestSize - 1;
        }
    }

    NiWChar* pkResult = wcsncpy(pkDest, pkSrc, stCount);
    pkResult[stCount] = '\0';

    return pkResult;
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to append characters of a string.
inline NiWChar* NiWStrcat(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc)
{

#if _MSC_VER >= 1400
    wcscat_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
    return pkDest;
#else // #if _MSC_VER >= 1400
    size_t stSrcLen = wcslen(pkSrc);
    size_t stDestLen = wcslen(pkDest);
    NIASSERT(stSrcLen + stDestLen <= stDestSize - 1);

    size_t stWrite = stDestSize - 1 - stDestLen;  

    NiWChar* pkResult = wcsncat(pkDest, pkSrc, stWrite);
    pkResult[stDestSize - 1] = '\0';
    return pkResult;

#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to find the next token in a string.
inline NiWChar* NiWStrtok(NiWChar* pkString, const NiWChar* pkDelimit, 
    NiWChar** ppkContext)
{

#if _MSC_VER >= 1400
    return (NiWChar *)wcstok_s((wchar_t *)pkString,
        (const wchar_t *)pkDelimit, (wchar_t **)ppkContext);
#else // #if _MSC_VER >= 1400
    return wcstok(pkString, pkDelimit);
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to write formatted output using a pointer to a
// list of arguments.
inline int NiWSprintf(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkFormat, ...)
{

    if (stDestSize == 0)
        return 0;

    va_list kArgs;
    va_start(kArgs, pkFormat);

#if _MSC_VER >= 1400
    int iRet = vswprintf_s((wchar_t *)pkDest, stDestSize,
        (const wchar_t *)pkFormat, kArgs);
#else // #if _MSC_VER >= 1400
    int iRet = vswprintf(pkDest, stDestSize, pkFormat, kArgs);

    if (iRet >= 0 && ((unsigned int)iRet == stDestSize - 1) &&
        pkDest[stDestSize - 1] != '\0')
    {
        // This is a rare case where the written string fits but
        // is not null terminated. We will report this as an error.
        iRet = -1;
    }
#endif // #if _MSC_VER >= 1400

    va_end(kArgs);
       
    pkDest[stDestSize - 1] = '\0';

    return iRet;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
inline void NiWriteToStdErr(char* pText)
{
    fputs(pText, stderr);
}
//---------------------------------------------------------------------------
#ifndef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
inline void NiOutputDebugString(const char* pcOut)
{
    OutputDebugStringA(pcOut);
}
#endif // #ifndef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
#endif
//---------------------------------------------------------------------------
