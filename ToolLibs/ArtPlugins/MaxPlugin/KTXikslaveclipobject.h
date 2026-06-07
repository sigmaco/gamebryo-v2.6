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
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#include "Max.h"
#include "plugapi.h"
#pragma warning(pop)

#define IKSLAVE_CLASSID Class_ID(0xbe380a31,0x310dc9e4)


class IKClipObject; // Sorry, can't show you this
class IKSlave;  // Sorry, can't show you this
class IKSlaveClipObject : public IKClipObject
{
public:     
    DWORD flags;
    float min[6], max[6], damping[6], spring[6], stens[6];
    
    enum {
        XPOS_ACTIVE  = (1<<0),
        YPOS_ACTIVE  = (1<<1),
        ZPOS_ACTIVE  = (1<<2),
        XROT_ACTIVE  = (1<<3),
        YROT_ACTIVE  = (1<<4),
        ZROT_ACTIVE  = (1<<5),
        XPOS_LIMITED = (1<<6),
        YPOS_LIMITED = (1<<7),
        ZPOS_LIMITED = (1<<8),
        XROT_LIMITED = (1<<9),
        YROT_LIMITED = (1<<10),
        ZROT_LIMITED = (1<<11),
        XPOS_EASE    = (1<<12),
        YPOS_EASE    = (1<<13),
        ZPOS_EASE    = (1<<14),
        XROT_EASE    = (1<<15),
        YROT_EASE    = (1<<16),
        ZROT_EASE    = (1<<17),
        XPOS_SPRING  = (1<<18),
        YPOS_SPRING  = (1<<19),
        ZPOS_SPRING  = (1<<20),
        XROT_SPRING  = (1<<21),
        YROT_SPRING  = (1<<22),
        ZROT_SPRING  = (1<<23)
    };

    IKSlaveClipObject(IKSlave *slave);
    inline SClass_ID SuperClassID() {return CTRL_MATRIX3_CLASS_ID;}
    inline Class_ID ClassID() {return IKSLAVE_CLASSID;}        
    inline void DeleteThis() {delete this;}
    inline bool Active(int i) {return (flags&(XPOS_ACTIVE<<i))?true:false;}
    inline bool Limited(int i) {return (flags&(XPOS_LIMITED<<i))?true:false;}
    inline bool Ease(int i) {return (flags&(XPOS_EASE<<i))?true:false;}
    inline bool Spring(int i) {return (flags&(XPOS_SPRING<<i))?true:false;}

};
