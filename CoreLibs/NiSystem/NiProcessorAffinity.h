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

#ifndef NIPROCESSORAFFINITY_H
#define NIPROCESSORAFFINITY_H

#include "NiSystemLibType.h"

class NISYSTEM_ENTRY NiProcessorAffinity{
public:
    enum Processor
    {
        PROCESSOR_0                     = 0x00000001,
        PROCESSOR_1                     = 0x00000002,
        PROCESSOR_2                     = 0x00000004,
        PROCESSOR_3                     = 0x00000008,
        PROCESSOR_4                     = 0x00000010,
        PROCESSOR_5                     = 0x00000020,
        PROCESSOR_6                     = 0x00000040,
        PROCESSOR_7                     = 0x00000080,
        PROCESSOR_8                     = 0x00000100,
        PROCESSOR_9                     = 0x00000200,
        PROCESSOR_10                    = 0x00000400,
        PROCESSOR_11                    = 0x00000800,
        PROCESSOR_12                    = 0x00001000,
        PROCESSOR_13                    = 0x00002000,
        PROCESSOR_14                    = 0x00004000,
        PROCESSOR_15                    = 0x00008000,
        PROCESSOR_16                    = 0x00010000,
        PROCESSOR_17                    = 0x00020000,
        PROCESSOR_18                    = 0x00040000,
        PROCESSOR_19                    = 0x00080000,
        PROCESSOR_20                    = 0x00100000,
        PROCESSOR_21                    = 0x00200000,
        PROCESSOR_22                    = 0x00400000,
        PROCESSOR_23                    = 0x00800000,
        PROCESSOR_24                    = 0x01000000,
        PROCESSOR_25                    = 0x02000000,
        PROCESSOR_26                    = 0x04000000,
        PROCESSOR_27                    = 0x08000000,
        PROCESSOR_28                    = 0x10000000,
        PROCESSOR_29                    = 0x20000000,
        PROCESSOR_30                    = 0x40000000,
        PROCESSOR_31                    = 0x80000000,

        // Xenon processors
        PROCESSOR_XENON_CORE_0_THREAD_0 = 0x00000001,
        PROCESSOR_XENON_CORE_0_THREAD_1 = 0x00000002,
        PROCESSOR_XENON_CORE_1_THREAD_0 = 0x00000004,
        PROCESSOR_XENON_CORE_1_THREAD_1 = 0x00000008,
        PROCESSOR_XENON_CORE_2_THREAD_0 = 0x00000010,
        PROCESSOR_XENON_CORE_2_THREAD_1 = 0x00000020,

        PROCESSOR_DONT_CARE             = 0xFFFFFFFF
    };

    NiProcessorAffinity();
    NiProcessorAffinity(Processor uiIdealProcessor,
        unsigned int uiAffinityMask);

    inline unsigned int GetAffinityMask() const;
    inline void SetAffinityMask(unsigned int uiAffinityMask);

    Processor GetIdealProcessor() const;
    inline void SetIdealProcessor(Processor eProcessor);

private:
    Processor m_eIdealProcessor;
    unsigned int m_uiAffinityMask;
};

#include "NiProcessorAffinity.inl"

#endif //#ifndef NIPROCESSORAFFINITY_H
