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

#ifndef STATUSCHECKING_H
#define STATUSCHECKING_H

#define REQUIRE_IOSTREAM 1

#include <Maya/MStatus.h>

#define CHECK_AND_ASSERT(x)                 \
{                                           \
    NIASSERT((x == MStatus::kSuccess));     \
    CHECK_MSTATUS((x));                     \
}


#define VERIFY_MSTATUS_AND_RETURN(stat, returnValue)    \
{                                                       \
    MStatus temp = (stat);                              \
    bool test = (temp) == MStatus::kSuccess;            \
    NIASSERT(test)                                      \
    CHECK_MSTATUS_AND_RETURN((temp) ,(returnValue));    \
}                                                         


#define VERIFY_MSTATUS_AND_RETURN_IT(stat)    \
    VERIFY_MSTATUS_AND_RETURN((stat), (stat))

#define VERIFY_MSTATUS(stat)                                                \
{                                                                           \
    MStatus _VERIFY_MSTATUS_Stat = (stat);                                  \
    bool test = (_VERIFY_MSTATUS_Stat) == MStatus::kSuccess;                \
    CHECK_AND_ASSERT(test)                                                  \
}                                                         


#endif // STATUSCHECKING_H