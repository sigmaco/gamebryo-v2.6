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
#include "NiEntityPCH.h"

#include "NiResetPropertyDataCommandFactory.h"

#include <NiFixedString.h>
#include "NiEntityPropertyInterface.h"

#include "NiResetFloatDataCommand.h"
#include "NiResetBoolDataCommand.h"
#include "NiResetIntDataCommand.h"
#include "NiResetUIntDataCommand.h"
#include "NiResetShortDataCommand.h"
#include "NiResetUShortDataCommand.h"
#include "NiResetStringDataCommand.h"
#include "NiResetPoint2DataCommand.h"
#include "NiResetPoint3DataCommand.h"
#include "NiResetQuaternionDataCommand.h"
#include "NiResetMatrix3DataCommand.h"
#include "NiResetColorDataCommand.h"
#include "NiResetColorADataCommand.h"
#include "NiResetNiObjectPointerDataCommand.h"
#include "NiResetEntityPointerDataCommand.h"

//---------------------------------------------------------------------------
NiChangePropertyCommand*
    NiResetPropertyDataCommandFactory::GetResetDataCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName)
{
    NiFixedString kPrimitiveType;
    if (!pkEntityPropertyInterface ||
        !pkEntityPropertyInterface->GetPrimitiveType(kPropertyName,
        kPrimitiveType))
    {
        return NULL;
    }

    if (kPrimitiveType == NiEntityPropertyInterface::PT_FLOAT)
    {
        return NiNew NiResetFloatDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_BOOL)
    {
        return NiNew NiResetBoolDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_INT)
    {
        return NiNew NiResetIntDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_UINT)
    {
        return NiNew NiResetUIntDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_SHORT)
    {
        return NiNew NiResetShortDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_USHORT)
    {
        return NiNew NiResetUShortDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_STRING)
    {
        return NiNew NiResetStringDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT2)
    {
        return NiNew NiResetPoint2DataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT3)
    {
        return NiNew NiResetPoint3DataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_QUATERNION)
    {
        return NiNew NiResetQuaternionDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_MATRIX3)
    {
        return NiNew NiResetMatrix3DataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLOR)
    {
        return NiNew NiResetColorDataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_COLORA)
    {
        return NiNew NiResetColorADataCommand(pkEntityPropertyInterface,
            kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_NIOBJECTPOINTER)
    {
        return NiNew NiResetNiObjectPointerDataCommand(
            pkEntityPropertyInterface, kPropertyName);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_ENTITYPOINTER)
    {
        return NiNew NiResetEntityPointerDataCommand(
            pkEntityPropertyInterface, kPropertyName);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
