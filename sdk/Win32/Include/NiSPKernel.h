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

#ifndef NISPKERNEL_H
#define NISPKERNEL_H

#include <NiSystem.h>
#include <NiString.h>
#include "NiFloodgateLibType.h"
#include "NiSPWorkload.h"

class NiSPTask;
class NiSPPPUWorkload;

/**
    This class is the encapsulation of Floodgate kernels.
    
    A kernel is a set of instructions that operate on one or more NiSPStream
    objects of varying or fixed data. Applications must always declare and
    define kernels using the macros found in NiSPKernelMacros.h.
    @ps3_begin For PPU kernels, look in NiSPPPUKernelMacros.h. @ps3_end
    See the Programmer's Guide section on
    Floodgate or the Mesh Creation demo for examples of kernel definitions. 
*/
class NIFLOODGATE_ENTRY NiSPKernel : public NiMemObject
{
public:

    /// Default constructor.
    inline NiSPKernel(){}
    
    /**
        Main constructor.

        @param pucName The name of the kernel.
    */
    inline NiSPKernel(const char* pucName) : m_strName(pucName) {}

    /// Destructor.
    inline virtual ~NiSPKernel(){}

    /// Returns the name of the kernel.
    inline const NiString& GetName() const { return m_strName; }

    /**
        Overloaded operator for assigning one kernel to another.

        @param kKernel Source kernel.
    */
    inline NiSPKernel& operator=(const NiSPKernel& kKernel)
    {
        m_strName = kKernel.m_strName;
        return *this;
    }
    
    /**
        Implements the work for the kernel.
        
        Classes derived from NiSPKernel typically declare an instance of this
        function implicitly using NiSPDeclareKernel and define it using
        NiSPBeginKernelImpl. See the Description section above for
        information on the Execute function of this kernel.

        @param kWorkload The workload to which this kernel belongs.
    */
    inline virtual void Execute(NiSPWorkload& kWorkload){NI_UNUSED_ARG(kWorkload);};
    
    ///
        
    /// @ps3_member
    /// Implements the work for a PPU kernel.
    ///
    /// Classes derived from NiSPKernel typically declare an instance of this
    /// function implicitly using NiSPDeclarePPUKernel or
    /// NiSPDeclarePPUKernelLib, and define it using
    /// NiSPBeginKernelImpl. See the Description section above for
    /// information on the Execute function of this kernel.
    inline virtual void Execute(NiSPPPUWorkload&){};
    /// @ps3_end

    /// @ps3_member
    /// Gets the starting memory address of the NiSPKernel to execute.
    /// This is not required on all platforms.
    inline virtual char* GetBinStart()
    {
        NIASSERT(0 && "Invalid call for this platform.");
        return NULL;
    }
    /// @ps3_end
    
    /// @ps3_member
    /// Gets the size of the NiSPKernel to execute.
    /// This is not required on all platforms.
    inline virtual char* GetBinSize()
    {
        NIASSERT(0 && "Invalid call for this platform.");
        return NULL;
    }
    /// @ps3_end

    /// @ps3_member
    /// Indicates if this is a PPU based Kernel. PS3 only.
    inline virtual bool IsPPUKernel(){ return false;}
    /// @ps3_end

protected:    
    /// The name of the kernel.
    NiString m_strName;
};

#endif  // #ifndef NISPKERNEL_H
