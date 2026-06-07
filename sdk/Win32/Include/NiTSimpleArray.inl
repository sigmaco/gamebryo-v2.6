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
template<typename T, size_t COUNT>
inline size_t NiTSimpleArray<T, COUNT>::Count() const
{
    return COUNT;
}
//---------------------------------------------------------------------------
template<typename T, size_t COUNT>
inline T& NiTSimpleArray<T, COUNT>::operator[](size_t stIndex)
{
    return m_kArray[stIndex];
}
//---------------------------------------------------------------------------
template<typename T, size_t COUNT>
inline const T& NiTSimpleArray<T, COUNT>::operator[](size_t stIndex) const
{
    return m_kArray[stIndex];
}
//---------------------------------------------------------------------------
