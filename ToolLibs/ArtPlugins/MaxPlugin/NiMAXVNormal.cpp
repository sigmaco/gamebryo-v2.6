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

#include "MaxImmerse.h"
#include "NiMAXVNormal.h"

//---------------------------------------------------------------------------
NiMAXVNormal::NiMAXVNormal() 
{
    m_lSmooth=0;
    m_pkNext=NULL;
    m_bInit=FALSE;
    m_kNorm=Point3(0,0,0);
} 
//---------------------------------------------------------------------------
NiMAXVNormal::NiMAXVNormal(const Point3 &normal,DWORD smooth) 
{
    m_pkNext=NULL;
    m_bInit=TRUE;
    m_kNorm=normal;
    m_lSmooth=smooth;
} 
//---------------------------------------------------------------------------
NiMAXVNormal::~NiMAXVNormal() 
{
} 
//---------------------------------------------------------------------------
void NiMAXVNormal::FreeNormals()
{
    NiMAXVNormal *pCurr = m_pkNext;
    while (pCurr)
    {
        NiMAXVNormal *pTemp = pCurr;
        pCurr = pCurr->m_pkNext;
        NiDelete pTemp;
    }
}
//---------------------------------------------------------------------------
void NiMAXVNormal::AddNormal(const Point3 &normal, DWORD smooth) 
{ 
    if (!(smooth & m_lSmooth) && m_bInit) 
    { 
        if (m_pkNext) 
        {
            m_pkNext->AddNormal(normal,smooth);
        }
        else 
        { 
            m_pkNext = NiNew NiMAXVNormal(normal,smooth); 
        }
    }
    else 
    { 
        m_kNorm += normal; 
        m_lSmooth |= smooth; 
        m_bInit = TRUE; 
    }
}
//---------------------------------------------------------------------------
Point3 &NiMAXVNormal::GetNormal(DWORD smooth) 
{ 
    if ((m_lSmooth & smooth) || !m_pkNext) 
        return m_kNorm; 
    else
        return m_pkNext->GetNormal(smooth);  
}
//---------------------------------------------------------------------------
void NiMAXVNormal::Normalize() 
{ 
    NiMAXVNormal *ptr = m_pkNext, *prev = this; 
    while (ptr) 
    {
        if (ptr->m_lSmooth & m_lSmooth) 
        { 
            m_kNorm += ptr->m_kNorm; 
            prev->m_pkNext = ptr->m_pkNext; 
            NiDelete ptr; 
            ptr = prev->m_pkNext; 
        } 
        else 
        { 
            prev = ptr; 
            ptr = ptr->m_pkNext; 
        } 
    } 

    m_kNorm = ::Normalize(m_kNorm); 
    if (m_pkNext) 
        m_pkNext->Normalize();
}
//---------------------------------------------------------------------------
