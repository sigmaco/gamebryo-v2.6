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
inline NiDefaultErrorHandler::Error::Error(const NiFixedString& kMessage,
    const NiFixedString& kDescription, const NiFixedString& kEntityName,
    const NiFixedString& kPropertyName) : m_kMessage(kMessage),
    m_kDescription(kDescription), m_kEntityName(kEntityName),
    m_kPropertyName(kPropertyName)
{
}
//---------------------------------------------------------------------------
