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

#ifndef NITADJUSTVALUE_H
#define NITADJUSTVALUE_H

#include <limits.h>
#include <NiFloat16.h>

// Template functions to adjust a value for clamping due to type.
// Double precision is used in order to handle Int32 ranges.
//---------------------------------------------------------------------------
template <typename TType> static inline TType AdjustValue(double dValue)
{
    // The default implementation applies to "unsigned" types. This
    // eliminates needing to specialize those types.
    if (dValue < 0.0f)
        return 0;
    else if (dValue >= (TType)-1)
        return (TType)-1;
    else
        return (TType)dValue;
};
//---------------------------------------------------------------------------
// Float32 specialization
template<> static inline float AdjustValue(double dValue)
{
    // float32 specialization
    return (float)dValue;
}
//---------------------------------------------------------------------------
// Float16 specialization
template<> static inline NiFloat16 AdjustValue(double dValue)
{
    // float16 specialization - clamp value to max limits
    if (dValue > FLOAT16_MAX)
        return (NiFloat16)FLOAT16_MAX;
    else if (dValue < -FLOAT16_MAX)
        return (NiFloat16)-FLOAT16_MAX;

    return (NiFloat16)((float)dValue);
}
//---------------------------------------------------------------------------
// "signed" types
template<> static inline NiInt32 AdjustValue(double dValue)
{
    if (dValue < INT_MIN)
        return INT_MIN;
    else if (dValue > INT_MAX)
        return INT_MAX;
    else
        return (NiInt32)dValue;
}
//---------------------------------------------------------------------------
template<> static inline NiInt16 AdjustValue(double dValue)
{
    if (dValue < SHRT_MIN)
        return SHRT_MIN;
    else if (dValue > SHRT_MAX)
        return SHRT_MAX;
    else
        return (NiInt16)dValue;
}
//---------------------------------------------------------------------------
template<> static inline NiInt8 AdjustValue(double dValue)
{
    if (dValue < SCHAR_MIN)
        return SCHAR_MIN;
    else if (dValue > SCHAR_MAX)
        return SCHAR_MAX;
    else
        return (NiInt8)dValue;
}
//---------------------------------------------------------------------------
template <typename T> static inline bool IsFloatingPoint()
{
    return false;
}
//---------------------------------------------------------------------------
template<> static inline bool IsFloatingPoint<float>()
{
    return true;
}
//---------------------------------------------------------------------------
template<> static inline bool IsFloatingPoint<NiFloat16>()
{
    return true;
}
//---------------------------------------------------------------------------
template <typename T> static inline T GetTypeRange(double& dMin, double& dMax)
{
    // Default is for unsigned types
    dMin = (double)0;
    dMax = (double)((T)-1);
    return 1;
}
//---------------------------------------------------------------------------
template<> static inline NiInt32 GetTypeRange(double& dMin, double& dMax)
{
    dMin = (double)INT_MIN;
    dMax = (double)INT_MAX;
    return 1;
}
//---------------------------------------------------------------------------
template<> static inline NiInt16 GetTypeRange(double& dMin, double& dMax)
{
    dMin = (double)SHRT_MIN;
    dMax = (double)SHRT_MAX;
    return 1;
}
//---------------------------------------------------------------------------
template<> static inline NiInt8 GetTypeRange(double& dMin, double& dMax)
{
    dMin = (double)SCHAR_MIN;
    dMax = (double)SCHAR_MAX;
    return 1;
}
//---------------------------------------------------------------------------
static inline void GetEnumTypeRange(NiDataStreamElement::Type eType, 
    double& dMin, double& dMax)
{
    switch (eType)
    {
    case NiDataStreamElement::T_INT8:
        GetTypeRange<NiInt8>(dMin, dMax);
        break;
    case NiDataStreamElement::T_INT16:
        GetTypeRange<NiInt16>(dMin, dMax);
        break;
    case NiDataStreamElement::T_INT32:
        GetTypeRange<NiInt32>(dMin, dMax);
        break;
    case NiDataStreamElement::T_UINT8:
        GetTypeRange<NiUInt8>(dMin, dMax);
        break;
    case NiDataStreamElement::T_UINT16:
        GetTypeRange<NiUInt16>(dMin, dMax);
        break;
    case NiDataStreamElement::T_UINT32:
        GetTypeRange<NiUInt32>(dMin, dMax);
        break;
    case NiDataStreamElement::T_FLOAT32:
    case NiDataStreamElement::T_FLOAT16:
    default:
        break;
    }
}
//---------------------------------------------------------------------------
static inline bool EnumIsSigned(NiDataStreamElement::Type eType)
{
    if ((eType == NiDataStreamElement::T_UINT8) ||
        (eType == NiDataStreamElement::T_UINT16) ||
        (eType == NiDataStreamElement::T_UINT32))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
static inline bool EnumIsFloatingPoint(NiDataStreamElement::Type eType)
{
    if ((eType == NiDataStreamElement::T_FLOAT16) ||
        (eType == NiDataStreamElement::T_FLOAT32))
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
#endif // NITADJUSTVALUE_H