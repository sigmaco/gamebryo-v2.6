// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net


// Copyright (c) 2005-2007 Intel Corporation 
// All Rights Reserved

#include "NiSystemPCH.h"

// Detects three forms of hardware multi-threading support across IA-32 
// platform The three forms of HW multithreading are: Multi-processor, 
// Multi-core, and  HyperThreading Technology.
//
// This code enumerates all the logical processors enabled by OS and BIOS,
// determine the HW topology of these enabled logical processors in the system 
// using information provided by CPUID instruction.
// 
// A multi-processing system can support any combination of the three forms of 
// HW multi-threading support. The relevant topology can be identified using a 
// three level decomposition of the "initial APIC ID" into 
// Package_id, core_id, and SMT_id. Such decomposition provides a three-level 
// map of the topology of hardware resources and allow multi-threaded software 
// to manage shared hardware resources in  the platform to reduce resource 
// contention
//
// Multicore detection algorithm for processor and cache topology requires
// all leaf functions of CPUID instructions be available. System administrator
// must ensure BIOS settings is not configured to restrict CPUID 
// functionalities.
//
// CPUID instruction is not available to most 486 class and earlier processors
// Executing CPUID instruction on 486 and earlier processors can cause 
// abnormal termination. Code examples to detect early IA-32 processors can 
// be found in AP-485 "Intel Processor Identification and the CPUID 
// instruction"
//
// CPUID instruction in Intel (R) Pentium (R) Pro, Pentium (R) III Xeon 
// processors do not provide all of the CPUID data leaves necessary to 
// enumerate three-level processor topology. Use this reference code only on 
// processors later than Pentium (R) 4 processor and Intel (R) Xeon (R) 
// processors.
//
// Enumerating cache topology is feasible only on processors that support 
// CPUID leaf 4
//----------------------------------------------------------------------------

// EDX[28]  Bit 28 is set if HT or multi-core is supported
#define HWD_MT_BIT         0x10000000     

// EBX[23:16] Bit 16-23 in ebx contains the number of logical
// processors per physical processor when execute cpuid with 
// eax set to 1
#define NUM_LOGICAL_BITS   0x00FF0000     
                                          
// EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
// per physical processor when execute cpuid with eax set to 4. 
#define NUM_CORE_BITS      0xFC000000     

// EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
// initial APIC ID for the processor this code is running on.
#define INITIAL_APIC_ID_BITS  0xFF000000  
                                          
#define MAX_CACHE_INDEX_TO        6        

// Status Flag
#define SINGLE_CORE_AND_HT_ENABLED              1
#define SINGLE_CORE_AND_HT_DISABLED             2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE          4
#define MULTI_CORE_AND_HT_ENABLED               5
#define MULTI_CORE_AND_HT_DISABLED              6
#define MULTI_CORE_AND_HT_NOT_CAPABLE           7
#define SET_THREAD_AFFINITY_ERROR               8

#include <stdio.h>
#include <windows.h>

//
// Initialize CPUID code path.  Determine if CPUID is a vaild instruction.
//
NiUInt32 NiSystemDesc::CPUID_Init()
{
    NiUInt32  CacheNum = 0;

    unsigned char StatusFlag = 0; 

    if ( !CPUID_CpuIDSupported() ) 
    { 
        // This processor does not support CPUID. No need to evaluate 
        // processor topology.
        return 1;
    }
    if( !CPUID_CheckCPU_ExtendedFamilyModel() ) 
    {
        // The CPUID in this processor may not provide sufficient data to 
        // evaluate three-level processor topology.
        return 2;
    }
    
       NiUInt32 index = 0, RegEAX = 0, TypeCache, LevelCache, MaxLPSharingCache, 
        LastIndex = 0;
    
    // CpuIDSupported() returning < 4 is single threaded processors
    // CpuIDSupported() will return 5 if P4 HT detected
    if (CPUID_CpuIDSupported() >= 4) 
    { 
        RegEAX = CPUID_QueryCacheType(index);        
        
        //locate the last valid index of a cache level
        do 
        {
            TypeCache = (RegEAX & 0x1f);
            LevelCache = (RegEAX & 0xe0)>>5;
            MaxLPSharingCache = 1 + ((RegEAX & 0x3ffc000)>>14);
            LastIndex = index;
            index ++;
            RegEAX = CPUID_QueryCacheType(index);        
            
            if( index > MAX_CACHE_INDEX_TO) 
                break;
        }
        while 
            ( (RegEAX & 0x1f) );
    }

    StatusFlag = CPUID_CPUCount(
        &m_uiNumLogicalProcessors,
        &m_uiNumPhysicalCores,
        &m_uiNumPhysicalProcessors,
        &CacheNum,
        LastIndex);

    if (StatusFlag == SET_THREAD_AFFINITY_ERROR)
    {
        // Failed to set thread affinity
        return 3;
    }
    
    NIASSERT (m_uiNumPhysicalProcessors * 
        CPUID_MaxCorePerPhysicalProc() >= m_uiNumPhysicalCores);
    NIASSERT (m_uiNumPhysicalProcessors * 
        CPUID_MaxLogicalProcPerPhysicalProc() >= m_uiNumLogicalProcessors);
  
    return 0;
}


//
// CpuIDSupported returns 0 if CPUID instruction is unavailable. 
// Otherwise, it will return the maximum number of regular data leaves 
// supported in this processor.
// CpuIDSupported provides two purposes:    
//     (a) Allow the program to bail when run on a processor that does have 
//          CPUID
//     (b) Allow software to determine if CPUID on this process provides 
//          sufficient data leaves to execute three-level processor topology 
//          enumeration algorithm.
//
NiUInt32 NiSystemDesc::CPUID_CpuIDSupported(void)
{
    NiUInt32 MaxInputValue;
    // The CPUID instruction is not supported in many 486 class 
    // and earlier processors and may cause abnormal termination.
    // AP-485 provides examples of detecting 486, 386 and 286 processors.
    // For simplicity, we simply omit any exception handling here since
    // Gamebryo requires a Pentium processor or better and does not use
    // exception handling.
    MaxInputValue = 0;
    
    // call cpuid with eax = 0
    __asm
    {
        xor eax, eax
        cpuid
        mov MaxInputValue, eax
    }

    return MaxInputValue;
}

//
// GenuineIntel will return 0 if the processor is not a Genuine Intel 
// Processor 
//
NiUInt32 NiSystemDesc::CPUID_GenuineIntel(void)
{
    NiUInt32 VendorID[3] = {0, 0, 0};
        
    // Get vendor id string
    __asm        
    {
        xor eax, eax            // call cpuid with eax = 0
           cpuid                    // Get vendor id string
        mov VendorID, ebx
        mov VendorID + 4, edx
        mov VendorID + 8, ecx
    }

    return ( (VendorID[0] == 'uneG') &&
             (VendorID[1] == 'Ieni') &&
             (VendorID[2] == 'letn')); 
}


// Return 0 if a check indicates the processor's CPUID does not provide 
// sufficient data leaves for three-level topology enumeration.
// Return 1 if the check found no issues with the processor's family/model, 
// extended family/model The purpose is to detect situations of older 
// processors that support CPUID, yet their CPUID had reserved bit fields 
// that on newer processors became critical pieces of raw data needed for 
// three-level processor topology enumeration. 
NiUInt32 NiSystemDesc::CPUID_CheckCPU_ExtendedFamilyModel(void)
{
    NiUInt32 Regeax      = 0;
    unsigned char ExtFam=0, ExtModel=0, Fam= 0, Model = 0;
    
    __asm
    {
            mov eax, 1
            cpuid
            mov Regeax, eax
    };

    ExtFam = static_cast<unsigned char>((Regeax >> 20) & 0xff);
    ExtModel = static_cast<unsigned char>((Regeax >> 16) & 0x0f);
    Fam = static_cast<unsigned char>((Regeax >>8) & 0x0f);
    Model = static_cast<unsigned char>((Regeax >>4) & 0x0f);
    
    // Processors based on Intel NetBurst Microarchitecture
    if( !ExtFam && Fam == 15) 
        return 1; 
    // Processors based on Core and Core2 Microarchtecture    
    if (!ExtFam && !ExtModel && Fam == 6 && 
        (Model == 15 || Model == 14 || Model == 13 || Model == 9) ) 
        return 1; 
    // Forward compatibility
    if (!ExtFam && ExtModel && Fam == 6) 
        return 1;  
    
    return 0;  
}


//
// Function returns the maximum cores per physical package. Note that the 
// number of AVAILABLE cores per physical to be used by an application might 
// be less than this maximum value.
//
NiUInt32 NiSystemDesc::CPUID_MaxCorePerPhysicalProc(void)
{
   
    NiUInt32 Regeax        = 0;
    
    // Single core
    if (!CPUID_HWD_MTSupported()) 
        return (NiUInt32) 1;  

    __asm
    {
        xor eax, eax
        cpuid
        cmp eax, 4            // check if cpuid supports leaf 4
        jl single_core1        // Single core
        mov eax, 4            
        mov ecx, 0            // start with index = 0; Leaf 4 reports
        cpuid                // at least one valid cache level
        mov Regeax, eax
        jmp multi_core

        single_core1:
            mov Regeax, 1

        multi_core:
    }
    
    return (NiUInt32)((Regeax & NUM_CORE_BITS) >> 26)+1;
}

//
// Return the value of EAX register for CPUID leaf 4, specify index value 
// from input argument
//
NiUInt32 NiSystemDesc::CPUID_QueryCacheType(NiUInt32 Index)
{
   
    NiUInt32 Regeax = 0;
    NiUInt32 lv = Index;
    
    if (!CPUID_HWD_MTSupported()) 
        return (NiUInt32) 1;  // Single core

    __asm
    {
        xor eax, eax
        cpuid
        cmp eax, 4            // check if cpuid supports leaf 4
        jl single_core2        // Single core
        mov eax, 4            
        mov ecx, lv             
        cpuid                // at least one valid cache level
        mov Regeax, eax
    
        single_core2:
    }

    return (NiUInt32)(Regeax ) ;
}


//
// The function returns 0 when the hardware multi-threaded bit is not set.
//
NiUInt32 NiSystemDesc::CPUID_HWD_MTSupported(void)
{
   
    NiUInt32 Regedx = 0;

    if ((CPUID_CpuIDSupported() >= 1) && CPUID_GenuineIntel())
    {
        __asm
        {
            mov eax, 1
            cpuid
            mov Regedx, edx
        }
    }
    return (Regedx & HWD_MT_BIT);  
}
               
//
// The function returns 0 when the mmx bit is not set.
//
NiUInt32 NiSystemDesc::CPUID_MMX_Supported(void)
{
   
    NiUInt32 Regedx = 0;

    if ((CPUID_CpuIDSupported() >= 1))
    {
        __asm
        {
            mov eax, 1
            cpuid
            mov Regedx, edx
        }
    }
    return ((Regedx >> 23) & 0x1);  
}
//
// The function returns 0 when the SSE bit is not set.
//
NiUInt32 NiSystemDesc::CPUID_SSE_Supported(void)
{
   
    NiUInt32 Regedx = 0;

    if ((CPUID_CpuIDSupported() >= 1))
    {
        __asm
        {
            mov eax, 1
            cpuid
            mov Regedx, edx
        }
    }
    return ((Regedx >> 25) & 0x1);  
}
//
// The function returns 0 when the SSE2 bit is not set.
//
NiUInt32 NiSystemDesc::CPUID_SSE2_Supported(void)
{
   
    NiUInt32 Regedx = 0;

    if ((CPUID_CpuIDSupported() >= 1))
    {
        __asm
        {
            mov eax, 1
            cpuid
            mov Regedx, edx
        }
    }
    return ((Regedx >> 26) & 0x1);  
}
//
// Function returns the maximum logical processors per physical package. 
// Note that the number of AVAILABLE logical processors per physical to be 
// used by an application might be less than this maximum value.
//
NiUInt32 NiSystemDesc::CPUID_MaxLogicalProcPerPhysicalProc(void)
{  
    // CPUID.01H:EBX is reserved for early processors, for example Pentium III 
    //processors

    NiUInt32 Regebx = 0;

    if (!CPUID_HWD_MTSupported()) return (NiUInt32) 1;
    __asm
    {
        mov eax, 1
        cpuid
        mov Regebx, ebx
    }
    return (NiUInt32) ((Regebx & NUM_LOGICAL_BITS) >> 16);
}

//
// The bit fields corresponding to Initial_APIC_ID is not defined for some 
// early processors, such as Pentium Pro or Pentium III Xeon processors
//
unsigned char NiSystemDesc::CPUID_GetAPIC_ID(void)
{    

    NiUInt32 Regebx = 0;
    __asm
    {
        mov eax, 1
        cpuid
        mov Regebx, ebx
    }
    
    return (unsigned char) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);
}


//
// Determine the width of the bit field that can represent the value 
// CountItem.
//
NiUInt32 NiSystemDesc::CPUID_Find_Maskwidth(NiUInt32 CountItem)
{
    NiUInt32 MaskWidth = 0xffffffff, count = CountItem;
    __asm
    {
        mov eax, count
        mov ecx, 0
        mov MaskWidth, ecx
        dec eax
        bsr cx, ax
        jz next
        inc cx
        mov MaskWidth, ecx

        next:
        
    }
    
    return MaskWidth;
}


//
// Extract the subset of bit field from the 8-bit value FullID.  
// It returns the 8-bit sub ID value
//
unsigned char NiSystemDesc::CPUID_GetNzbSubID(unsigned char FullID,
                          unsigned char MaxSubIDValue,
                          unsigned char ShiftCount)
{
    NiUInt32 MaskWidth;
    unsigned char MaskBits;

    MaskWidth = CPUID_Find_Maskwidth((NiUInt32) MaxSubIDValue);
    MaskBits  = (0xff << ShiftCount) ^ 
                ((unsigned char) (0xff << (ShiftCount + MaskWidth)));

    return (FullID & MaskBits);
}

//
// The big function that determines physical processors and cores
//
unsigned char NiSystemDesc::CPUID_CPUCount(NiUInt32 *TotAvailLogical,
                       NiUInt32 *TotAvailCore,
                       NiUInt32 *PhysicalNum,
                       NiUInt32 *CacheNum,
                       NiUInt32 LastIndex)
{
    unsigned char StatusFlag = 0;
    NiUInt32 numLPEnabled = 0;
    DWORD dwAffinityMask;
    int j = 0;
    NiUInt32 MaxLPPerCore, MaxNumLPSharingCache;
    unsigned char apicID, PackageIDMask, CacheIDMask;
    unsigned char tblPkgID[256], tblCoreID[256], tblSMTID[256], 
        tblCacheID[256];

    *TotAvailCore = 1;
    *PhysicalNum  = 1;
    *CacheNum  = 1;

    DWORD dwProcessAffinity, dwSystemAffinity;
    GetProcessAffinityMask(GetCurrentProcess(), 
                           &dwProcessAffinity,
                           &dwSystemAffinity);

    // In rare cases, dwProcessAffinity may not match dwSystemAffinity if 
    // someone is modifying affinity masks.  This will provide information 
    // about the detected cores that this process has access to.

    // Assume that cores within a package have the SAME number of 
    // logical processors.  Also, values returned by
    // MaxLogicalProcPerPhysicalProc and MaxCorePerPhysicalProc do not have
    // to be power of 2.

    MaxLPPerCore = CPUID_MaxLogicalProcPerPhysicalProc() / 
        CPUID_MaxCorePerPhysicalProc();
    dwAffinityMask = 1;

    while (dwAffinityMask && dwAffinityMask <= dwProcessAffinity)
    {
        // Check to see if we can set the thread affinity
        if ( !SetThreadAffinityMask(GetCurrentThread(), dwAffinityMask) )
        {
            return SET_THREAD_AFFINITY_ERROR;
        }
        else
        {
            Sleep(0);  // Ensure system to switch to the right CPU
            apicID = CPUID_GetAPIC_ID();

            // Store SMT ID and core ID of each logical processor
            // Shift vlaue for SMT ID is 0
            tblSMTID[j]  = CPUID_GetNzbSubID(apicID, 
                (unsigned char) MaxLPPerCore, 0);  
            
            // Shift value for core ID is the mask width for maximum
            // logical processors per core
            tblCoreID[j] = CPUID_GetNzbSubID(apicID, 
                (unsigned char) CPUID_MaxCorePerPhysicalProc(),
                (unsigned char) CPUID_Find_Maskwidth(MaxLPPerCore));

            // Extract package ID, assume single cluster.
            PackageIDMask = (unsigned char) (0xff << 
                CPUID_Find_Maskwidth(CPUID_MaxLogicalProcPerPhysicalProc()));
            tblPkgID[j] = apicID & PackageIDMask;
            MaxNumLPSharingCache = 
                (( CPUID_QueryCacheType(LastIndex) &0x3ffc000) >> 14) + 1;
            CacheIDMask = 
                ((unsigned char)(0xff << 
                CPUID_Find_Maskwidth(MaxNumLPSharingCache)));
            tblCacheID[j] = apicID & CacheIDMask ; 

            // Number of available logical processors in the system.
            numLPEnabled ++;   

        } // else
        j++;
        dwAffinityMask = 1 << j;
    } // while

    // restore the affinity setting to its original state
    SetThreadAffinityMask(GetCurrentThread(), dwProcessAffinity);
    Sleep(0);

    *TotAvailLogical = numLPEnabled;
    
    //
    // Count available cores (TotAvailCore) in the system
    //
    unsigned char CoreIDBucket[256];
    DWORD ProcessorMask, pCoreMask[256];
    NiUInt32 i, ProcessorNum;

    CoreIDBucket[0] = tblPkgID[0] | tblCoreID[0];
    ProcessorMask = 1;
    pCoreMask[0] = ProcessorMask;

    for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
    {
        ProcessorMask <<= 1;
        for (i = 0; i < *TotAvailCore; i++)
        {
            // Comparing bit-fields of logical processors residing in 
            // different packages Assuming the bit-masks are the same on all 
            // processors in the system.
            if ((tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum]) == 
                CoreIDBucket[i])
            {
                pCoreMask[i] |= ProcessorMask;
                break;
            }

        }  // for i

        // did not match any bucket.  Start a new one.
        if (i == *TotAvailCore)   
        {
            CoreIDBucket[i] = tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum];
            pCoreMask[i] = ProcessorMask;

            (*TotAvailCore)++;    // Number of available cores in the system
        }
    }  // for ProcessorNum

    //
    // Count physical processor (PhysicalNum) in the system
    //
    unsigned char PackageIDBucket[256];
    DWORD pPackageMask[256];

    PackageIDBucket[0] = tblPkgID[0];
    ProcessorMask = 1;
    pPackageMask[0] = ProcessorMask;

    for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
    {
        ProcessorMask <<= 1;
        for (i = 0; i < *PhysicalNum; i++)
        {
            // Comparing bit-fields of logical processors residing in 
            // different packages Assuming the bit-masks are the same on all 
            // processors in the system.
            if (tblPkgID[ProcessorNum]== PackageIDBucket[i])
            {
                pPackageMask[i] |= ProcessorMask;
                break;
            }
        }  // for i

        if (i == *PhysicalNum)   // did not match any bucket.  Start a new one.
        {    
            PackageIDBucket[i] = tblPkgID[ProcessorNum];
            pPackageMask[i] = ProcessorMask;

            // Total number of physical processors in the system 
            (*PhysicalNum)++;    
        }
    }

    //
    // Discover cache topology, each bit in pCacheMask[i] corresponds to a 
    // logical processor sharing the i'th target-level cache. 
    // processor number corrsponds to bit position.
    //
    
    /* INTEL_COMMENTS: This portion of code will determine cache information.  
    // Reinsert if cache information is desired.
    unsigned char CacheIDBucket[256];
    DWORD pCacheMask[256];

    CacheIDBucket[0] = tblCacheID[0];
    ProcessorMask = 1;
    pCacheMask[0] = ProcessorMask;
    
    for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
    {
        ProcessorMask <<= 1;
        for (i = 0; i < *CacheNum; i++)
        {
            // Comparing bit-fields of logical processors residing in 
            // different packages Assuming the bit-masks are the same on all 
            // processors in the system.
            if (tblCacheID[ProcessorNum]== CacheIDBucket[i])
            {    
                pCacheMask[i] |= ProcessorMask;
                break;
            }
        }  // for i

        if (i == *CacheNum)   // did not match any bucket.  Start a new one.
        {    
            CacheIDBucket[i] = tblCacheID[ProcessorNum];
            pCacheMask[i] = ProcessorMask;

            (*CacheNum)++;    // Total number of target-level cache in the system
        }
    }  // for ProcessorNum
    */ // END INTEL_COMMENTS for Cache

    //
    // Check to see if the system is multi-core and/or SMT
    //
    if (*TotAvailCore > *PhysicalNum) 
    {
        // Multi-core
        if (MaxLPPerCore == 1)
            StatusFlag = MULTI_CORE_AND_HT_NOT_CAPABLE;
        else if (numLPEnabled > *TotAvailCore)
            StatusFlag = MULTI_CORE_AND_HT_ENABLED;
        else 
            StatusFlag = MULTI_CORE_AND_HT_DISABLED;
    }
    else
    {
        // Single-core
        if (MaxLPPerCore == 1)
            StatusFlag = SINGLE_CORE_AND_HT_NOT_CAPABLE;
        else if (numLPEnabled > *TotAvailCore)
            StatusFlag = SINGLE_CORE_AND_HT_ENABLED;
        else 
            StatusFlag = SINGLE_CORE_AND_HT_DISABLED;
    }

    return StatusFlag;
}
