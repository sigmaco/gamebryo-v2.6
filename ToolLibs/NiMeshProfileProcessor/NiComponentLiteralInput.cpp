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

#include <NiSystem.h>
#include "NiComponentLiteralInput.h"
#include "NiMeshProfileOperations.h"

//---------------------------------------------------------------------------
bool NiComponentLiteralInput::AssignBasis(StreamBasis&) const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::IsCompatible(const StreamBasis&) const
{
    // Literals are always compatible.
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::SetInput(double dLiteral)
{
    m_dLiteral = dLiteral;
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, float& val)
{
    val = AdjustValue<float>(m_dLiteral);
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiFloat16& val)
{
    val = AdjustValue<NiFloat16>(m_dLiteral);
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiUInt32& val)
{
    val = AdjustValue<NiUInt32>(m_dLiteral);
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiUInt16& val)
{
    val = AdjustValue<NiUInt16>(m_dLiteral); 
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiUInt8& val)
{
    val = AdjustValue<NiUInt8>(m_dLiteral); 
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiInt32& val)
{
    val = AdjustValue<NiInt32>(m_dLiteral); 
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiInt16& val)
{
    val = AdjustValue<NiInt16>(m_dLiteral); 
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentLiteralInput::GetValue(NiUInt32, NiInt8& val)
{
    val = AdjustValue<NiInt8>(m_dLiteral); 
    return true;
}
//---------------------------------------------------------------------------

