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

//  NiMeshProfileComponent inline functions

//---------------------------------------------------------------------------
inline NiMeshProfileComponent::Component NiMeshProfileComponent::GetType()
    const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline const char* NiMeshProfileComponent::GetLinkedComponent() const
{
    return m_strLinkedComponent;
}
//---------------------------------------------------------------------------
inline void NiMeshProfileComponent::SetLinkedComponent(
    const char* pcLinkedComponent)
{
    m_strLinkedComponent = pcLinkedComponent;
}
//---------------------------------------------------------------------------
inline const NiMeshProfileComponent::NiRemap& 
    NiMeshProfileComponent::GetRemap() const
{
    return m_kRemap;
}
//---------------------------------------------------------------------------
inline const NiMeshProfileComponent::NiClamp& 
    NiMeshProfileComponent::GetClamp() const
{
    return m_kClamp;
}
//---------------------------------------------------------------------------
inline NiMeshProfileComponent::Component
    NiMeshProfileComponent::GetComponentType(const char c)
{
    switch (c)
    {
    case 'x':
    case 'X':
        return PD_X;

    case 'y':
    case 'Y':
        return PD_Y;

    case 'z':
    case 'Z':
        return PD_Z;

    case 'w':
    case 'W':
        return PD_W;

    case 'r':
    case 'R':
        return PD_R;

    case 'g':
    case 'G':
        return PD_G;

    case 'b':
    case 'B':
        return PD_B;

    case 'a':
    case 'A':
        return PD_A;
    
    case '0':
        return PD_0;

    case '1':
        return PD_1;

    case '2':
        return PD_2;

    case '3':
        return PD_3;

    case 'u':
    case 'U':
        return PD_U;

    case 'v':
    case 'V':
        return PD_V;
    }

    return PD_DISABLED;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfileComponent::NeedsClamp() const
{
    return m_kClamp.m_bIsValueSet;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfileComponent::NeedsRemap() const
{
    return m_kRemap.m_bIsValueSet;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfileComponent::IsLiteral() const
{
    return (NiMeshProfileComponent::IsLiteral(m_strLinkedComponent));
}
//---------------------------------------------------------------------------
