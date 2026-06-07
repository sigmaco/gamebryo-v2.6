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
//  NiTriShape inline functions

//---------------------------------------------------------------------------
inline unsigned int NiTriShape::GetTriListLength() const
{
    NiTriShapeData* pkData = NiSmartPointerCast(NiTriShapeData,m_spModelData);
    return pkData->GetTriListLength();
}
//---------------------------------------------------------------------------
inline unsigned short* NiTriShape::GetTriList() const
{
    NiTriShapeData* pkData = NiSmartPointerCast(NiTriShapeData,m_spModelData);
    return pkData->GetTriList();
}
//---------------------------------------------------------------------------
