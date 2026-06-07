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

#ifndef NIMEMHINT_H
#define NIMEMHINT_H

#include "NiSystemLibType.h"

//---------------------------------------------------------------------------
// NiMemHint
//
// Indicates the intended usage for the memory being allocated which allows
// the allocator to optionally pick the most efficient memory type or location,
// as well as providing additional information to the memory tracker when enabled.
//---------------------------------------------------------------------------

#define NIHINTFIELD(shift, value) ((value)<<(shift))

class NISYSTEM_ENTRY NiMemHint
{
public:
    enum Bits
    {
        // Bits 7..0 are per-platform memory hints; although 00000000 is
        // reserved to mean no hint was given
        PLATFORM_NONE = NIHINTFIELD(0, 0x00),
    #if defined(_WII)
        // Require memory to be allocated from Wii's MEM1 or MEM2 arena
        WII_MEM1 = NIHINTFIELD(0, 0x10),
        WII_MEM2 = NIHINTFIELD(0, 0x11),

        // Request memory to be allocated from Wii's MEM1 or MEM2 arena, falling
        // back to the opposite arena if the request could not be satisified
        WII_TRY_MEM1 = NIHINTFIELD(0, 0x12),
        WII_TRY_MEM2 = NIHINTFIELD(0, 0x13),
    #elif defined(_PS3)
        // Note: these settings are not valid for standard allocations, they
        // are only for allocations occuring through the NiPS3GPUMemoryManager.
        PS3_MAIN          = NIHINTFIELD(0, 0x20),
        PS3_RSX_LOCAL     = NIHINTFIELD(0, 0x21),
        PS3_TRY_MAIN      = NIHINTFIELD(0, 0x22),
        PS3_TRY_RSX_LOCAL = NIHINTFIELD(0, 0x23),
    #endif

        // Bits 9..8 provide lifetime information
        LIFETIME_APPLICATION = NIHINTFIELD(8, 0),
        LIFETIME_LONG        = NIHINTFIELD(8, 1),
        LIFETIME_SHORT       = NIHINTFIELD(8, 2),
        LIFETIME_FRAME       = NIHINTFIELD(8, 3),

        // Bit 10
        // If true, the compiler will pass in the allocation size on deallocation
        // or reallocation calls, so it does not have to be stored explicitly.
        COMPILER_PROVIDES_SIZE_ON_DEALLOCATE = NIHINTFIELD(10, 1),

        // Bit 15..11 - Access types
        ACCESS_CPU      = NIHINTFIELD(11, 0),
        ACCESS_VOLATILE = NIHINTFIELD(11, 1),
        ACCESS_MUTABLE  = NIHINTFIELD(11, 2),
        ACCESS_STATIC   = NIHINTFIELD(11, 3),
        ACCESS_GPU      = NIHINTFIELD(11, 4),

        // Bit 21..16 - Usage hints
        USAGE_UNKNOWN = NIHINTFIELD(16, 0),
        USAGE_POOL    = NIHINTFIELD(16, 1),
        USAGE_TEXTURE = NIHINTFIELD(16, 2),
        USAGE_AUDIO   = NIHINTFIELD(16, 3),
        USAGE_VIDEO   = NIHINTFIELD(16, 4),
        USAGE_INDEX   = NIHINTFIELD(16, 5),
        USAGE_VERTEX  = NIHINTFIELD(16, 6),
        USAGE_PHYSICS = NIHINTFIELD(16, 7),
        
        // Bits 29..22 - Currently unused

        // Bit 30 indicates that the allocation is a bulk allocation instead of
        // a 'metadata' allocation (e.g., GPU visible data instead of NiTexture)
        IS_BULK = NIHINTFIELD(30, 1),

        // Bit 31 indicates how bits 30..16 behave
        TYPE_GB = NIHINTFIELD(31, 0),
        TYPE_CUSTOM = NIHINTFIELD(31, 1),

        // Common combinations
        NONE = 0x00000000,                 // Normal memory allocation
        TEXTURE = USAGE_TEXTURE | IS_BULK  // GPU texture allocation
    };

    // Returns the platform-specific hint (masks all but bits 7..0)
    inline Bits GetPlatform() const;

    // Returns the lifetime hint (masks all but bits 9..8)
    inline Bits GetLifetime() const;

    // Returns true if the hint specifies that the compiler will provide
    // an accurate size on deallocations (bit 10)
    inline bool CompilerProvidesSizeOnDeallocate() const;

    // Returns the access hint (masks all but bits 15..11)
    inline Bits GetAccess() const;

    // Returns the usage hint (masks all but bits 21..16)
    inline Bits GetUsage() const;

    // Returns true if the hint indicated that it is a bulk allocation (bit 30)
    inline bool IsBulk() const;

    // Returns true if the hint is a custom hint (if so, bits 30..16 are not
    // defined and not used by the standard allocators)
    inline bool IsCustomHint() const;

    // Returns the internal representation of the hint
    inline unsigned int GetRaw() const;

    // Constructors
    inline NiMemHint();
    inline NiMemHint(Bits eValue);

    // Operators
    inline NiMemHint& operator|=(const NiMemHint kRhs);
    inline NiMemHint& operator|=(const NiMemHint::Bits eRhs);
    inline NiMemHint operator|(const NiMemHint kRhs) const;
    inline NiMemHint operator|(const NiMemHint::Bits eRhs) const;
    inline bool operator==(const NiMemHint kRhs) const;
    inline bool operator!=(const NiMemHint kRhs) const;
protected:
    // Actual storage for the hint 
    Bits m_eValue;
};

inline NiMemHint::Bits operator|(const NiMemHint::Bits eLhs, const NiMemHint::Bits eRhs);

#undef NIHINTFIELD


// This macro returns a 'default' hint that gives no additional information
#define NI_MEMHINT_NONE NiMemHint()

//---------------------------------------------------------------------------

#include "NiMemHint.inl"

#endif // #ifndef NIMEMHINT_H
