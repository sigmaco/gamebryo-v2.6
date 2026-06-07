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
inline NiUInt32 NiPhysXFluidPropDesc::GetNumFluids() const
{
    return m_kFluids.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXFluidDescPtr NiPhysXFluidPropDesc::GetFluidDesc(
    NiUInt32 uiIndex)
{
    return m_kFluids.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidPropDesc::AddFluidDesc(NiPhysXFluidDesc* pkFluidDesc)
{
    m_kFluids.AddFirstEmpty(pkFluidDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXFluidDescPtr NiPhysXFluidPropDesc::RemoveFluidDesc(
    NiUInt32 uiIndex)
{
    return m_kFluids.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidPropDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
