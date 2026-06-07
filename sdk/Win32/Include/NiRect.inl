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
//  NiRect inline functions

//---------------------------------------------------------------------------
template <class T>
inline void NiRect<T>::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,m_left);
    NiStreamLoadBinary(stream,m_right);
    NiStreamLoadBinary(stream,m_top);
    NiStreamLoadBinary(stream,m_bottom);
}
//---------------------------------------------------------------------------
template <class T>
inline void NiRect<T>::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,m_left);
    NiStreamSaveBinary(stream,m_right);
    NiStreamSaveBinary(stream,m_top);
    NiStreamSaveBinary(stream,m_bottom);
}
//---------------------------------------------------------------------------
template <class T>
inline char* NiRect<T>::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 64;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = (L=%g,R=%g,T=%g,B=%g)", pPrefix, 
        (float)m_left, (float)m_right, (float)m_top, (float)m_bottom);

    return pString;
}
//---------------------------------------------------------------------------
template <class T>
inline T NiRect<T>::GetWidth() const
{
    return m_right > m_left ? (m_right - m_left) : (m_left - m_right);
}
//---------------------------------------------------------------------------
template <class T>
inline T NiRect<T>::GetHeight() const
{
    return m_top > m_bottom ? (m_top - m_bottom) : (m_bottom - m_top);
}
//---------------------------------------------------------------------------
